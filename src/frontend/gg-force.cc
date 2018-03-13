/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <getopt.h>
#include <iostream>
#include <vector>
#include <thread>
#include <sys/time.h>
#include <sys/resource.h>

#include "execution/reductor.hh"
#include "net/s3.hh"
#include "storage/backend_local.hh"
#include "storage/backend_s3.hh"
#include "thunk/ggutils.hh"
#include "thunk/placeholder.hh"
#include "thunk/thunk_reader.hh"
#include "thunk/thunk.hh"
#include "tui/status_bar.hh"
#include "util/digest.hh"
#include "util/exception.hh"
#include "util/optional.hh"
#include "util/path.hh"
#include "util/timeit.hh"
#include "util/util.hh"

using namespace std;
using namespace gg::thunk;

void usage( const char * argv0 )
{
  cerr << "Usage: " << argv0 << " [options] THUNKS..." << endl
       << endl
       << "Options:" << endl
       << " -j, --jobs    maximum number of jobs to run in parallel" << endl
       << " -s, --status  show the status bar for the job" << endl
       << " -T, --timeout number of seconds before duplicating running jobs" << endl
       << endl
       << "Useful environment variables:" << endl
       << "  GG_SANDBOXED => if set, forces the thunks in a sandbox" << endl
       << "  GG_REMOTE    => execute the thunks on AWS Lambda" << endl
       << endl;
}

void check_rlimit_nofile( const size_t max_jobs )
{
  struct rlimit limits;
  CheckSystemCall( "getrlimit", getrlimit( RLIMIT_NOFILE, &limits ) );

  size_t target_nofile = max( max_jobs * 2, limits.rlim_cur );
  target_nofile = min( target_nofile, limits.rlim_max );

  if ( limits.rlim_cur < target_nofile ) {
    cerr << "Increasing the maximum number of allowed file descriptors from "
         << limits.rlim_cur << " to " << target_nofile << ".\n";
    limits.rlim_cur = target_nofile;
    CheckSystemCall( "setrlimit", setrlimit( RLIMIT_NOFILE, &limits ) );
  }
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

    const bool lambda_execution = ( getenv( "GG_LAMBDA" ) != NULL );
    const bool ggremote_execution = ( getenv( "GG_REMOTE" ) != NULL );
    size_t max_jobs = thread::hardware_concurrency();
    bool status_bar = false;
    int timeout = -1;

    struct option long_options[] = {
      { "status", no_argument, nullptr, 's' },
      { "jobs", required_argument, nullptr, 'j' },
      { "timeout", required_argument, nullptr, 'T' },
      { nullptr, 0, nullptr, 0 },
    };

    while ( true ) {
      const int opt = getopt_long( argc, argv, "sj:T:", long_options, NULL );

      if ( opt == -1 ) {
        break;
      }

      switch ( opt ) {
      case 's':
        status_bar = true;
        StatusBar::get();
        break;

      case 'j':
        max_jobs = stoul( optarg );
        break;

      case 'T':
        timeout = stoi( optarg );
        break;

      default:
        throw runtime_error( "invalid option" );
      }
    }

    check_rlimit_nofile( max_jobs );

    gg::models::init();

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
          thunk_hash = gg::hash::compute( target_filename, gg::ObjectType::Thunk );
        }
      }
      else {
        thunk_hash = placeholder->content_hash();
      }

      target_hashes.emplace_back( move( thunk_hash ) );
    }

    vector<ExecutionEnvironment> execution_environments;
    unique_ptr<StorageBackend> storage_backend;

    if ( lambda_execution ) {
      execution_environments.push_back( ExecutionEnvironment::LAMBDA );
    }

    if ( ggremote_execution ) {
      execution_environments.push_back( ExecutionEnvironment::GG_RUNNER );
    }

    if ( not ( lambda_execution or ggremote_execution ) ) {
      execution_environments.push_back( ExecutionEnvironment::LOCAL );
    }

    if ( lambda_execution or ggremote_execution ) {
      storage_backend = StorageBackend::create_backend( gg::remote::storage_backend_uri() );
    }

    Reductor reductor { target_hashes, max_jobs,
                        execution_environments,
                        move( storage_backend ),
                        ( timeout > 0 ) ? ( timeout * 1000 ) : -1,
                        status_bar };

    reductor.upload_dependencies();
    vector<string> reduced_hashes = reductor.reduce();
    reductor.download_targets( reduced_hashes );

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
