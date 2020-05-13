/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "reductor.hh"

#include <iomanip>
#include <sstream>
#include <iostream>
#include <cmath>
#include <numeric>
#include <chrono>

#include "thunk/ggutils.hh"
#include "thunk/thunk_reader.hh"
#include "net/s3.hh"
#include "tui/status_bar.hh"
#include "util/optional.hh"
#include "util/exception.hh"
#include "util/timeit.hh"
#include "util/path.hh"
#include "util/digest.hh"

using namespace std;
using namespace gg;
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
  if ( not status_bar_ ) {
    return;
  }

  static time_point<steady_clock> last_display = steady_clock::now();
  const static string color_reset = COLOR_RESET"\033[48;5;236m";

  const auto this_display = steady_clock::now();

  if ( duration_cast<milliseconds>( this_display - last_display ).count() > 33 ) {
    last_display = this_display;

    ostringstream data;

    data << color_reset
         << "in queue: " << BOLD << COLOR_YELLOW  << setw( 5 ) << left
         << job_queue_.size() << color_reset;

    for ( auto & ee : exec_engines_ ) {
      data << " " << ee->label() << " (" << ee->max_jobs() << "): "
           << BOLD << COLOR_RED << setw( 5 ) << left << ee->job_count()
           << color_reset;
    }

    data << " done: "  << BOLD << COLOR_GREEN << setw( 5 ) << left
         << finished_jobs_ << color_reset
         << " remaining: " << BOLD << COLOR_DEFAULT << dep_graph_.size();

    data << "  |  cost: " << BOLD << COLOR_CYAN << "~$" << setw( 8 ) << fixed
         << setprecision( 2 ) << estimated_cost_;

    StatusBar::set_text( data.str() );
  }
}

void print_gg_message( const string & tag, const string & message )
{
  cerr << "[" << tag << "] " << message << endl;
}

Reductor::Reductor( const vector<string> & target_hashes,
                    vector<unique_ptr<ExecutionEngine>> && execution_engines,
                    vector<unique_ptr<ExecutionEngine>> && fallback_engines,
                    std::unique_ptr<StorageBackend> && storage_backend,
                    const std::chrono::milliseconds default_timeout,
                    const size_t timeout_multiplier,
                    const bool status_bar )
  : target_hashes_( target_hashes ),
    remaining_targets_(),
    status_bar_( status_bar ), default_timeout_( default_timeout ),
    timeout_multiplier_( timeout_multiplier ),
    exec_engines_( move( execution_engines ) ),
    fallback_engines_( move( fallback_engines ) ),
    storage_backend_( move( storage_backend ) )
{
  cerr << "\u2192 Loading the thunks... ";
  auto graph_load_time = time_it<milliseconds>(
    [this] ()
    {
      for ( const string & hash : target_hashes_ ) {
        string inserted_hash = dep_graph_.add_thunk( hash );

        if ( remaining_targets_.count( inserted_hash ) == 0) {
          remaining_targets_.insert( move ( inserted_hash ) );
          unordered_set<string> thunk_o1_deps = dep_graph_.order_one_dependencies( hash );
          job_queue_.insert( job_queue_.end(), thunk_o1_deps.begin(), thunk_o1_deps.end() );
        }
      }
    } ).count();

  cerr << " done (" << graph_load_time << " ms)." << endl;

  auto success_callback =
    [this] ( const string & old_hash, vector<ThunkOutput> && outputs, const float cost )
    { finalize_execution( old_hash, move( outputs ), cost ); };

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


  if ( exec_engines_.size() == 0 ) {
    throw runtime_error( "no execution engines are available" );
  }

  for ( auto & ee : exec_engines_ ) {
    ee->set_success_callback( success_callback );
    ee->set_failure_callback( failure_callback );
    ee->init( exec_loop_ );
  }

  for ( auto & fe : fallback_engines_ ) {
    fe->set_success_callback( success_callback );
    fe->set_failure_callback( failure_callback );
    fe->init( exec_loop_ );
  }
}

void Reductor::finalize_execution( const string & old_hash,
                                   vector<ThunkOutput> && outputs,
                                   const float cost )
{
  running_jobs_.erase( old_hash );
  const string main_output_hash = outputs.at( 0 ).hash;

  Optional<unordered_set<string>> new_o1s = dep_graph_.force_thunk( old_hash, move ( outputs ) );
  estimated_cost_ += cost;

  if ( new_o1s.initialized() ) {
    job_queue_.insert( job_queue_.end(), new_o1s->begin(), new_o1s->end() );

    if ( gg::hash::type( main_output_hash ) == gg::ObjectType::Value ) {
      remaining_targets_.erase( dep_graph_.original_hash( old_hash ) );
    }

    finished_jobs_++;
  }
}

vector<string> Reductor::reduce()
{
  while ( true ) {
    while ( not job_queue_.empty() ) {
      print_status();

      const string thunk_hash { move( job_queue_.front() ) };
      job_queue_.pop_front();

      /* don't bother executing gg-execute if it's in the cache */
      Optional<ReductionResult> cache_entry;

      while ( true ) {
        auto temp_cache_entry = gg::cache::check( cache_entry.initialized() ? cache_entry->hash
                                                                            : thunk_hash );

        if ( temp_cache_entry.initialized() ) {
          cache_entry = move( temp_cache_entry );
        }
        else {
          break;
        }
      }

      if ( cache_entry.initialized() ) {
        Thunk thunk { ThunkReader::read( gg::paths::blob( thunk_hash ), thunk_hash ) };
        vector<ThunkOutput> new_outputs;

        for ( const auto & tag : thunk.outputs() ) {
          Optional<cache::ReductionResult> result = cache::check( gg::hash::for_output( thunk_hash, tag ) );

          if ( not result.initialized() ) {
            throw runtime_error( "inconsistent cache entries" );
          }

          new_outputs.emplace_back( result->hash, tag );
        }

        finalize_execution( thunk_hash, move( new_outputs ), 0 );
      }
      else {
        const Thunk & thunk = dep_graph_.get_thunk( thunk_hash );

        enum { CANNOT_BE_EXECUTED,
               FULL_CAPACITY,
               FULL_FALLBACK_CAPACITY,
               EXECUTING } exec_state = CANNOT_BE_EXECUTED;

        for ( auto & exec_engine : exec_engines_ ) {
          if ( exec_engine->can_execute( thunk ) ) {
            if ( exec_engine->job_count() >= exec_engine->max_jobs() ) {
              exec_state = FULL_CAPACITY;
              continue;
            }

            exec_engine->force_thunk( thunk, exec_loop_ );
            exec_state = EXECUTING;
            break;
          }
        }

        /* the job cannot be executed on any of the execution engines */
        if ( exec_state == CANNOT_BE_EXECUTED ) {
          for ( auto & fallback_engine : fallback_engines_ ) {
            if ( fallback_engine->can_execute( thunk ) ) {
              if ( fallback_engine->job_count() >= fallback_engine->max_jobs() ) {
                exec_state = FULL_FALLBACK_CAPACITY;
                continue;
              }

              fallback_engine->force_thunk( thunk, exec_loop_ );
              exec_state = EXECUTING;
              break;
            }
          }
        }

        if ( exec_state == EXECUTING ) {
          JobInfo & job_info = running_jobs_[ thunk_hash ];
          job_info.start = Clock::now();
          job_info.timeout = thunk.timeout() * timeout_multiplier_;
          job_info.restarts++;

          if ( job_info.timeout == 0s ) {
            job_info.timeout = default_timeout_;
          }
        }
        else if ( exec_state == FULL_CAPACITY or exec_state == FULL_FALLBACK_CAPACITY ) {
          job_queue_.push_front( thunk_hash );
          break;
        }
        else { /* CANNOT_BE_EXECUTED */
          throw runtime_error( "no execution engine could execute " + thunk_hash );
        }
      }
    } /* while(Q is not empty) */

    print_status();

    const auto poll_result = exec_loop_.loop_once( timeout_check_interval_ == 0s
                                                   ? -1
                                                   : timeout_check_interval_.count() );
    const auto clock_now = Clock::now();

    if ( timeout_check_interval_ != 0s and clock_now >= next_timeout_check_ ) {
      size_t count = 0;

      for ( auto & job : running_jobs_ ) {
        if ( job.second.timeout != 0ms and
             ( clock_now - job.second.start ) > job.second.timeout ) {
          job_queue_.push_back( job.first );
          job.second.start = clock_now;
          job.second.timeout += job.second.restarts * job.second.timeout;
          job.second.restarts++;

          count ++;
        }
      }

      next_timeout_check_ += timeout_check_interval_;

      if ( count > 0 ) {
        print_gg_message( "info", "duplicating " + to_string( count ) +
                                  " job" + ( ( count == 1 ) ? "" : "s" ) );
      }
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
  size_t total_size = 0;

  for ( const string & dep : dep_graph_.value_dependencies() ) {
    if ( storage_backend_->is_available( dep ) ) {
      continue;
    }

    total_size += gg::hash::size( dep );
    upload_requests.push_back( { gg::paths::blob( dep ), dep,
                                 gg::hash::to_hex( dep ) } );
  }

  for ( const string & dep : dep_graph_.executable_dependencies() ) {
    if ( storage_backend_->is_available( dep ) ) {
      continue;
    }

    total_size += gg::hash::size( dep );
    upload_requests.push_back( { gg::paths::blob( dep ), dep,
                                 gg::hash::to_hex( dep ) } );
  }

  if ( upload_requests.size() == 0 ) {
    cerr << "No files to upload." << endl;
    return;
  }

  const string plural = upload_requests.size() == 1 ? "" : "s";
  cerr << "\u2197 Uploading " << upload_requests.size() << " file" << plural
       << " (" << format_bytes( total_size ) << ")... ";

  auto upload_time = time_it<milliseconds>(
    [&upload_requests, this]()
    {
      storage_backend_->put(
        upload_requests,
        [this] ( const storage::PutRequest & upload_request )
        { storage_backend_->set_available( upload_request.object_key ); }
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
  size_t total_size = 0;

  for ( const string & hash : hashes ) {
    if ( not roost::exists( gg::paths::blob( hash ) ) ) {
      download_requests.push_back( { hash, gg::paths::blob( hash ) } );
      total_size += gg::hash::size( hash );
    }
  }

  if ( download_requests.size() == 0 ) {
    cerr << "No files to download." << endl;
    return;
  }

  const string plural = download_requests.size() == 1 ? "" : "s";
  cerr << "\u2198 Downloading output file" << plural
       << " (" << format_bytes( total_size ) << ")... ";
  auto download_time = time_it<milliseconds>(
    [&download_requests, this]()
    {
      storage_backend_->get( download_requests );
    }
  );

  cerr << "done (" << download_time.count() << " ms)." << endl;
}
