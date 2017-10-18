/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <unistd.h>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <getopt.h>
#include <iostream>
#include <sstream>
#include <memory>
#include <unordered_set>
#include <algorithm>
#include <deque>
#include <list>
#include <vector>
#include <iomanip>
#include <thread>
#include <numeric>
#include <csignal>
#include <sys/ioctl.h>

#include "exception.hh"
#include "thunk.hh"
#include "thunk_reader.hh"
#include "child_process.hh"
#include "sandbox.hh"
#include "path.hh"
#include "temp_file.hh"
#include "temp_dir.hh"
#include "thunk_writer.hh"
#include "ggpaths.hh"
#include "placeholder.hh"
#include "system_runner.hh"
#include "graph.hh"
#include "poller.hh"
#include "signalfd.hh"
#include "util.hh"
#include "s3.hh"
#include "digest.hh"
#include "remote_response.hh"
#include "secure_socket.hh"
#include "optional.hh"
#include "units.hh"
#include "timeit.hh"
#include "status_bar.hh"
#include "loop.hh"
#include "engine.hh"
#include "engine_local.hh"
#include "engine_lambda.hh"

using namespace std;
using namespace gg::thunk;
using namespace PollerShortNames;

using ReductionResult = gg::cache::ReductionResult;
using SSLConnectionState = SSLConnectionContext::State;

bool status_bar = false;
const bool sandboxed = ( getenv( "GG_SANDBOXED" ) != NULL );
const bool lambda_execution = ( getenv( "GG_LAMBDA" ) != NULL );
const bool ggremote_execution = ( getenv( "GG_REMOTE" ) != NULL );

enum class ExecutionEnvironment { GG_RUNNER, LAMBDA };

#define COLOR_DEFAULT "\033[39m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_CYAN    "\033[36m"
#define BOLD          "\033[1m"
#define COLOR_RESET   "\033[0m"

class Reductor
{
private:
  const vector<string> target_hashes_;
  size_t max_jobs_;

  ExecutionLoop exec_loop_ {};
  deque<string> job_queue_ {};

  vector<unique_ptr<ExecutionEngine>> exec_engines_;

  size_t finished_jobs_ { 0 };

  DependencyGraph dep_graph_ {};

  void execution_finalize( const string & old_hash, const string & new_hash );

  size_t running_jobs() const;
  bool is_finished() const;

public:
  Reductor( const vector<string> & target_hashes, const size_t max_jobs = 8 );

  vector<string> reduce();
  void upload_dependencies() const;
  void print_status() const;
};

void Reductor::print_status() const
{
  if ( not status_bar ) {
    return;
  }

  static time_t last_display = 0;
  const static string color_reset = COLOR_RESET"\033[48;5;236m";

  time_t this_display = time( nullptr );

  if ( this_display != last_display ) {
    last_display = this_display;

    ostringstream data;

    data << color_reset
         << "[" << setw( 3 ) << std::right    << ceil( 100 * finished_jobs_ / dep_graph_.size() ) << "%]"
         << " in queue: "    << BOLD << COLOR_YELLOW  << setw( 5 ) << std::left << job_queue_.size()   << color_reset;

    for ( auto & ee : exec_engines_ ) {
      data << " " << ee->label() << ": " << BOLD << COLOR_RED << setw( 5 )
           << std::left << ee->job_count() << color_reset;
    }

    data << " done: "  << BOLD << COLOR_GREEN << setw( 5 ) << std::left << finished_jobs_ << color_reset
         << " total: " << BOLD << COLOR_DEFAULT << dep_graph_.size();

    StatusBar::set_text( data.str() );
  }
}

Reductor::Reductor( const vector<string> & target_hashes, const size_t max_jobs )
  : target_hashes_( target_hashes ), max_jobs_( max_jobs ), exec_engines_()
{
  unordered_set<string> all_o1_deps;

  for ( const string & hash : target_hashes_ ) {
    dep_graph_.add_thunk( hash );

    unordered_set<string> thunk_o1_deps = dep_graph_.order_one_dependencies( hash );
    all_o1_deps.insert( thunk_o1_deps.begin(), thunk_o1_deps.end() );
  }

  job_queue_.insert( job_queue_.end(), all_o1_deps.begin(), all_o1_deps.end() );

  auto completion_callback =
    [this] ( const string & old_hash, const string & new_hash )
    { execution_finalize( old_hash, new_hash ); };

  if ( lambda_execution ) {
    exec_engines_.emplace_back(
      make_unique<AWSLambdaExecutionEngine>(
        AWSCredentials(), gg::remote::s3_region(), exec_loop_, completion_callback
      )
    );
  }

  if ( not lambda_execution ) {
    exec_engines_.emplace_back(
      make_unique<LocalExecutionEngine>( exec_loop_, completion_callback )
    );
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
  return
    ( running_jobs() == 0 ) and ( job_queue_.size() == 0 );
}

void Reductor::execution_finalize( const string & old_hash, const string & new_hash )
{
  finished_jobs_++;
  unordered_set<string> new_o1s = dep_graph_.force_thunk( old_hash, new_hash );
  job_queue_.insert( job_queue_.end(), new_o1s.begin(), new_o1s.end() );
}

vector<string> Reductor::reduce()
{
  while ( true ) {
    while ( not job_queue_.empty() and running_jobs() < max_jobs_ ) {
      const string & thunk_hash = job_queue_.front();

      /* don't bother executing gg-execute if it's in the cache */
      Optional<ReductionResult> cache_entry = gg::cache::check( thunk_hash );
      if ( cache_entry.initialized() ) {
        unordered_set<string> new_o1s = dep_graph_.force_thunk( thunk_hash, cache_entry->hash );
        job_queue_.insert( job_queue_.end(), new_o1s.begin(), new_o1s.end() );
      }
      else {
        const Thunk & thunk = dep_graph_.get_thunk( thunk_hash );
        exec_engines_[ 0 ]->force_thunk( thunk_hash, thunk );
      }

      job_queue_.pop_front();
    }

    print_status();

    const auto poll_result = exec_loop_.loop_once();

    if ( poll_result.result == Poller::Result::Type::Exit ) {
      if ( not is_finished() ) {
        throw runtime_error( "poller failure happened, job is not finished" );
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
  vector<S3::UploadRequest> upload_requests;

  for ( const string & dep : dep_graph_.order_zero_dependencies() ) {
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
    [&upload_requests]()
    {
      S3ClientConfig s3_config;
      s3_config.region = gg::remote::s3_region();

      S3Client s3_client { s3_config };
      s3_client.upload_files(
        gg::remote::s3_bucket(), upload_requests,
        [] ( const S3::UploadRequest & upload_request )
        { gg::remote::set_available( upload_request.object_key ); }
      );
    }
  );

  cerr << "done (" << upload_time.count() << " ms)." << endl;
}

void usage( const char * argv0 )
{
  cerr << argv0 << " THUNK [execution args]" << endl
       << endl
       << "Useful environment variables:" << endl
       << "  GG_DIR       => absolute path to gg directory" << endl
       << "  GG_SANDBOXED => if set, forces the thunks in a sandbox" << endl
       << "  GG_MAXJOBS   => maximum number of jobs to run in parallel" << endl
       << "  GG_REMOTE    => execute the thunks on AWS Lambda" << endl
       << endl;
}

int main( int argc, char * argv[] )
{
  try {
    if ( argc <= 0 ) {
      abort();
    }

    if ( argc < 2 ) {
      usage( argv[ 0 ] );
      return EXIT_FAILURE;
    }

    struct option long_options[] = {
      { "status", no_argument, nullptr, 's' },
      { nullptr, 0, nullptr, 0 },
    };

    while ( true ) {
      const int opt = getopt_long( argc, argv, "s", long_options, NULL );

      if ( opt == -1 ) {
        break;
      }

      switch ( opt ) {
      case 's':
        status_bar = true;
        break;

      default:
        throw runtime_error( "invalid option" );
      }
    }

    gg::models::init();

    size_t max_jobs = thread::hardware_concurrency();
    if ( getenv( "GG_MAXJOBS" ) != nullptr ) {
      max_jobs = stoul( safe_getenv( "GG_MAXJOBS" ) );
    }

    // string thunk_filename { argv[ 1 ] };
    // const roost::path thunk_path = roost::canonical( thunk_filename );

    vector<string> target_filenames;
    vector<string> target_hashes;

    for ( int i = optind; i < argc; i++ ) {
      target_filenames.emplace_back( argv[ i ] );
    }

    for ( const string & target_filename : target_filenames ) {
      string thunk_hash;

      /* first check if this file is actually a placeholder */
      Optional<ThunkPlaceholder> placeholder = ThunkPlaceholder::read( target_filename );

      if ( not placeholder.initialized() ) {
        ThunkReader thunk_reader { target_filename };

        if( not thunk_reader.is_thunk() ) {
          throw runtime_error( "not a thunk: " + target_filename );
        }
        else {
          thunk_hash = InFile::compute_hash( target_filename );
        }
      }
      else {
        thunk_hash = placeholder->content_hash();
      }

      target_hashes.emplace_back( move( thunk_hash ) );
    }

    Reductor reductor { target_hashes, max_jobs };

    if ( lambda_execution or ggremote_execution ) {
      reductor.upload_dependencies();
    }

    vector<string> reduced_hashes = reductor.reduce();

    if ( lambda_execution or ggremote_execution ) {
      /* we need to fetch the output from S3 */
      vector<S3::DownloadRequest> download_requests;
      for ( const string & hash : reduced_hashes ) {
        download_requests.push_back( { hash, gg::paths::blob_path( hash ) } );
      }

      cerr << "\u2198 Downloading output files... ";
      auto download_time = time_it<chrono::milliseconds>(
        [&download_requests]()
        {
          S3ClientConfig s3_config;
          s3_config.region = gg::remote::s3_region();

          S3Client s3_client { s3_config };
          s3_client.download_files( gg::remote::s3_bucket(), download_requests );
        }
      );

      cerr << "done (" << download_time.count() << " ms)." << endl;
    }

    for ( size_t i = 0; i < reduced_hashes.size(); i++ ) {
      roost::copy_then_rename( gg::paths::blob_path( reduced_hashes[ i ] ), target_filenames[ i ] );

      /* HACK this is a just a dirty hack... it's not always right */
      roost::make_executable( target_filenames[ i ] );
    }

    return EXIT_SUCCESS;
  }
  catch ( const exception &  e ) {
    print_exception( argv[ 0 ], e );
    return EXIT_FAILURE;
  }
}
