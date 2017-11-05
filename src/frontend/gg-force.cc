/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <getopt.h>
#include <iostream>
#include <vector>
#include <thread>
#include <sstream>

#include "exception.hh"
#include "thunk.hh"
#include "thunk_reader.hh"
#include "path.hh"
#include "ggpaths.hh"
#include "placeholder.hh"
#include "util.hh"
#include "s3.hh"
#include "digest.hh"
#include "optional.hh"
#include "timeit.hh"
#include "status_bar.hh"
#include "reductor.hh"
#include "system_runner.hh"

using namespace std;
using namespace gg::thunk;

void usage( const char * argv0 )
{
  cerr << argv0 << " THUNK [execution args]" << endl
       << endl
       << "Useful environment variables:" << endl
       << "  GG_DIR       => absolute path to gg directory" << endl
       << "  GG_SANDBOXED => if set, forces the thunks in a sandbox" << endl
       << "  GG_MAXJOBS   => maximum number of jobs to run in parallel" << endl
       << "  GG_REMOTE    => execute the thunks on AWS Lambda" << endl
       << "  GG_WHISK     => execute the thunks on OpenWhisk" << endl
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

    const bool lambda_execution = ( getenv( "GG_LAMBDA" ) != NULL );
    const bool ggremote_execution = ( getenv( "GG_REMOTE" ) != NULL );
    const bool wsk_execution = ( getenv( "GG_WHISK" ) != NULL );
    bool status_bar = false;

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
        StatusBar::get();
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

    vector<ExecutionEnvironment> execution_environments;

    if ( wsk_execution ) {
      execution_environments.push_back( ExecutionEnvironment::OPENWHISK );
    }

    if ( lambda_execution ) {
      execution_environments.push_back( ExecutionEnvironment::LAMBDA );
    }

    if ( ggremote_execution ) {
      execution_environments.push_back( ExecutionEnvironment::GG_RUNNER );
    }

    if ( not ( lambda_execution or ggremote_execution or wsk_execution ) ) {
      execution_environments.push_back( ExecutionEnvironment::LOCAL );
    }

    Reductor reductor { target_hashes, max_jobs, execution_environments, status_bar };

    if ( lambda_execution or ggremote_execution ) {
      reductor.upload_dependencies( StorageBackend::S3 );
    }

    if ( wsk_execution ) {
      reductor.upload_dependencies( StorageBackend::KKV );
    }

    vector<string> reduced_hashes = reductor.reduce();

    if ( lambda_execution or ggremote_execution or wsk_execution ) {
      /* we need to fetch the output from S3 */
      vector<S3::DownloadRequest> download_requests;
      for ( const string & hash : reduced_hashes ) {
        download_requests.push_back( { hash, gg::paths::blob_path( hash ) } );
      }

      cerr << "\u2198 Downloading output files... ";
      auto download_time = time_it<chrono::milliseconds>(
        [&]()
        {
          if ( ggremote_execution or lambda_execution ) {
            S3ClientConfig s3_config;
            s3_config.region = gg::remote::s3_region();

            S3Client s3_client { s3_config };
            s3_client.download_files( gg::remote::s3_bucket(), download_requests );
          }
          else {
            ostringstream download_oss;

            for ( const auto & req : download_requests ) {
              download_oss << req.object_key << " " << req.filename.string() << endl;
            }

            run_with_input( "kkv-download", { "kkv-download" }, download_oss.str() );
          }
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
