/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "reductor.hh"

#include <iomanip>
#include <sstream>
#include <iostream>
#include <cmath>
#include <numeric>
#include <chrono>

#include "engine_local.hh"
#include "engine_lambda.hh"
#include "engine_gg.hh"
#include "thunk/ggpaths.hh"
#include "net/s3.hh"
#include "tui/status_bar.hh"
#include "util/optional.hh"
#include "util/exception.hh"
#include "util/timeit.hh"
#include "util/path.hh"
#include "util/digest.hh"

using namespace std;
using namespace gg::thunk;
using namespace std::chrono;

using ReductionResult = gg::cache::ReductionResult;

#define COLOR_DEFAULT "\033[39m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_CYAN    "\033[36m"
#define BOLD          "\033[1m"
#define COLOR_RESET   "\033[0m"

void Reductor::print_status() const
{
  static time_point<steady_clock> last_display = steady_clock::now();
  const static string color_reset = COLOR_RESET"\033[48;5;236m";

  auto this_display = steady_clock::now();

  if ( duration_cast<milliseconds>( this_display - last_display ).count() > 10 ) {
    last_display = this_display;

    ostringstream data;

    data << color_reset
         << "[" << setw( 3 ) << right
         << ceil( 100 * finished_jobs_ / total_jobs_ ) << "\x25]"
         << " in queue: " << BOLD << COLOR_YELLOW  << setw( 5 ) << left
         << job_queue_.size() << color_reset;

    for ( auto & ee : exec_engines_ ) {
      data << " " << ee->label() << ": " << BOLD << COLOR_RED << setw( 5 )
           << left << ee->job_count() << color_reset;
    }

    data << " done: "  << BOLD << COLOR_GREEN << setw( 5 ) << left
         << finished_jobs_ << color_reset
         << " total: " << BOLD << COLOR_DEFAULT << total_jobs_;

    data << "  |  cost: " << BOLD << COLOR_CYAN << "~$" << setw( 8 ) << fixed
         << setprecision( 2 ) << estimated_cost_;

    StatusBar::set_text( data.str() );
  }
}

void print_gg_message( const string & tag, const string & message )
{
  cerr << "[" << tag << "] " << message << endl;
}

Reductor::Reductor( const vector<string> & target_hashes, const size_t max_jobs,
                    const vector<ExecutionEnvironment> & execution_environments,
                    std::unique_ptr<StorageBackend> && storage_backend,
                    const int base_timeout, const bool status_bar )
  : target_hashes_( target_hashes ),
    remaining_targets_( target_hashes_.begin(), target_hashes_.end() ),
    max_jobs_( max_jobs ), status_bar_( status_bar ),
    base_poller_timeout_( base_timeout ),
    poller_timeout_( base_timeout ),
    storage_backend_( move( storage_backend ) )
{
  unordered_set<string> all_o1_deps;

  for ( const string & hash : target_hashes_ ) {
    dep_graph_.add_thunk( hash );

    unordered_set<string> thunk_o1_deps = dep_graph_.order_one_dependencies( hash );
    all_o1_deps.insert( thunk_o1_deps.begin(), thunk_o1_deps.end() );
  }

  total_jobs_ = dep_graph_.size();

  job_queue_.insert( job_queue_.end(), all_o1_deps.begin(), all_o1_deps.end() );

  auto success_callback =
    [this] ( const string & old_hash, const string & new_hash, const float cost )
    { finalize_execution( old_hash, new_hash, cost ); };

  auto failure_callback =
    [this] ( const string & old_hash, const JobStatus failure_reason )
    {
      switch ( failure_reason ) {
      /* this is the only fatal failure */
      case JobStatus::ExecutionFailure:
        throw runtime_error( "execution failed: " + old_hash );

      /* for all of the following cases, except default, we will push the failed
      job back into the queue */
      case JobStatus::InvocationFailure:
        print_gg_message( "warning", "invocation failed: " + old_hash );
        break;

      case JobStatus::RateLimit:
        print_gg_message( "warning", "rate limited: " + old_hash );
        break;

      case JobStatus::FetchDependenciesFailure:
        print_gg_message( "warning", "fetching the dependencies failed: " + old_hash );
        break;

      case JobStatus::UploadOutputFailure:
        print_gg_message( "warning", "uploading the output failed: " + old_hash );
        break;

      case JobStatus::OperationalFailure:
        print_gg_message( "warning", "operational failure: " + old_hash );
        break;

      case JobStatus::SocketFailure:
        print_gg_message( "warning", "socket failure: " + old_hash );
        break;

      case JobStatus::ChildProcessFailure:
        print_gg_message( "warning", "child process failure: " + old_hash );
        break;

      default:
        throw runtime_error( "execution failed for an unknown reason: " + old_hash );
      }

      /* let's retry */
      job_queue_.push_back( old_hash );
    };

  for ( auto ee : execution_environments ) {
    switch ( ee ) {
    case ExecutionEnvironment::LOCAL:
      exec_engines_.emplace_back(
        make_unique<LocalExecutionEngine>( success_callback, failure_callback )
      );

      break;

    case ExecutionEnvironment::LAMBDA:
      exec_engines_.emplace_back(
        make_unique<AWSLambdaExecutionEngine>(
          AWSCredentials(), AWS::region(), success_callback,
          failure_callback
        )
      );

      break;

    case ExecutionEnvironment::GG_RUNNER:
      {
        auto runner_server = gg::remote::runner_server();

        exec_engines_.emplace_back(
          make_unique<GGExecutionEngine>(
            runner_server.first, runner_server.second, success_callback,
            failure_callback
          )
        );
      }
      break;
    }
  }

  if ( exec_engines_.size() == 0 ) {
    throw runtime_error( "no execution engines are available" );
  }
}

size_t Reductor::running_jobs() const
{
  return accumulate(
    exec_engines_.begin(), exec_engines_.end(), 0,
    []( int a, const unique_ptr<ExecutionEngine> & b )
    { return a + b->job_count(); }
  );
}

bool Reductor::is_finished() const
{
  return remaining_targets_.size() == 0;
}

void Reductor::finalize_execution( const string & old_hash,
                                   const string & new_hash,
                                   const float cost )
{
  running_jobs_.erase( old_hash );

  Optional<unordered_set<string>> new_o1s = dep_graph_.force_thunk( old_hash, new_hash );
  estimated_cost_ += cost;

  if ( new_o1s.initialized() ) {
    job_queue_.insert( job_queue_.end(), new_o1s->begin(), new_o1s->end() );
    remaining_targets_.erase( dep_graph_.original_hash( old_hash ) );
    finished_jobs_++;
  }
}

vector<string> Reductor::reduce()
{
  while ( true ) {
    while ( not job_queue_.empty() and running_jobs() < max_jobs_ ) {
      const string & thunk_hash = job_queue_.front();

      /* don't bother executing gg-execute if it's in the cache */
      Optional<ReductionResult> cache_entry = gg::cache::check( thunk_hash );
      if ( cache_entry.initialized() ) {
        finalize_execution( thunk_hash, cache_entry->hash, 0 );
      }
      else {
        const Thunk & thunk = dep_graph_.get_thunk( thunk_hash );

        bool executing = false;

        for ( auto & exec_engine : exec_engines_ ) {
          if ( exec_engine->can_execute( thunk ) ) {
            exec_engine->force_thunk( thunk, exec_loop_ );
            executing = true;
            break;
          }
        }

        if ( not executing ) {
          throw runtime_error( "no execution engine could execute " + thunk_hash );
        }

        running_jobs_.insert( thunk_hash );
      }

      job_queue_.pop_front();
    }

    if ( status_bar_ ) {
      print_status();
    }

    const auto poll_result = exec_loop_.loop_once( poller_timeout_ );

    if ( poll_result.result == Poller::Result::Type::Timeout
         and base_poller_timeout_ > 0 ) {
      /* poller has timed out. it means that during the alloted time
      no job was finished. So, we add every running job to the queue! */
      print_gg_message( "info", "no responses during last "
                        + to_string( poller_timeout_ / 1000 )
                        + "s, duplicating " + to_string( running_jobs_.size() )
                        + " job(s)." );

      job_queue_.insert( job_queue_.end(),
                         running_jobs_.begin(), running_jobs_.end() );
      poller_timeout_ = max( poller_timeout_ * base_poller_timeout_,
                             poller_timeout_ );
    }
    else {
      poller_timeout_ = base_poller_timeout_; /* return to the roots */
    }

    if ( is_finished() or poll_result.result == Poller::Result::Type::Exit ) {
      if ( not is_finished() ) {
        throw runtime_error( "unhandled poller failure happened, job is not finished" );
      }

      vector<string> final_hashes;

      for ( const string & target_hash : target_hashes_ ) {
        const string final_hash = dep_graph_.updated_hash( target_hash );
        const Optional<ReductionResult> answer = gg::cache::check( final_hash );
        if ( not answer.initialized() ) {
          throw runtime_error( "internal error: final answer not found for " + target_hash );
        }
        final_hashes.emplace_back( answer->hash );
      }

      return final_hashes;
    }
  }
}

void Reductor::upload_dependencies() const
{
  if ( storage_backend_ == nullptr ) {
    return;
  }

  vector<storage::PutRequest> upload_requests;

  for ( const string & dep : dep_graph_.value_dependencies() ) {
    if ( gg::remote::is_available( dep ) ) {
      continue;
    }

    upload_requests.push_back( { gg::paths::blob_path( dep ), dep,
                                 digest::gghash_to_hex( dep ) } );
  }

  for ( const string & dep : dep_graph_.executable_dependencies() ) {
    if ( gg::remote::is_available( dep ) ) {
      continue;
    }

    upload_requests.push_back( { gg::paths::blob_path( dep ), dep,
                                 digest::gghash_to_hex( dep ) } );
  }

  if ( upload_requests.size() == 0 ) {
    cerr << "No files to upload." << endl;
    return;
  }

  const string plural = upload_requests.size() == 1 ? "" : "s";
  cerr << "\u2197 Uploading " << upload_requests.size() << " file" << plural << "... ";

  auto upload_time = time_it<chrono::milliseconds>(
    [&upload_requests, this]()
    {
      storage_backend_->put(
        upload_requests,
        [] ( const storage::PutRequest & upload_request )
        { gg::remote::set_available( upload_request.object_key ); }
      );
    }
  );

  cerr << "done (" << upload_time.count() << " ms)." << endl;
}

void Reductor::download_targets( const vector<string> & hashes ) const
{
  if ( storage_backend_ == nullptr ) {
    return;
  }

  vector<storage::GetRequest> download_requests;
  for ( const string & hash : hashes ) {
    download_requests.push_back( { hash, gg::paths::blob_path( hash ) } );
  }

  cerr << "\u2198 Downloading output files... ";
  auto download_time = time_it<chrono::milliseconds>(
    [&download_requests, this]()
    {
      storage_backend_->get( download_requests );
    }
  );

  cerr << "done (" << download_time.count() << " ms)." << endl;
}
