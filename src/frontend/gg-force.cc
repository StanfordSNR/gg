/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <iostream>
#include <vector>
#include <thread>
#include <getopt.h>
#include <cstdlib>
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
#include "execution/engine.hh"
#include "execution/engine_local.hh"
#include "execution/engine_lambda.hh"
#include "execution/engine_gg.hh"
#include "execution/engine_meow.hh"
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
  cerr << "Usage: " << argv0 << endl
       << "       " << "[-j|--jobs=<N>] [-s|--status] [-T|--timeout=<t>] [-S|--sandboxed]" << endl
       << "       " << "[-e|--engine=<name>[=ENGINE_ARGS]]... [-d,--no-download]" << endl
       << "       " << "THUNKS..." << endl
       << endl
       << "Available engines:" << endl
       << "  - local   Executes the jobs on the local machine" << endl
       << "  - lambda  Executes the jobs on AWS Lambda" << endl
       << "  - remote  Executes the jobs on a remote machine" << endl
       << "  - meow    Executes the jobs on AWS Lambda with long-running workers" << endl
       << endl;
}

void check_rlimit_nofile( const size_t max_jobs )
{
  struct rlimit limits;
  CheckSystemCall( "getrlimit", getrlimit( RLIMIT_NOFILE, &limits ) );

  size_t target_nofile = max( max_jobs * 3, limits.rlim_cur );
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

    size_t max_jobs = thread::hardware_concurrency();
    bool status_bar = false;
    bool no_download = false;
    int timeout = -1;

    vector<pair<string, string>> engines;

    struct option long_options[] = {
      { "status",      no_argument,       nullptr, 's' },
      { "sandboxed",   no_argument,       nullptr, 'S' },
      { "jobs",        required_argument, nullptr, 'j' },
      { "timeout",     required_argument, nullptr, 'T' },
      { "engine",      required_argument, nullptr, 'e' },
      { "no-download", no_argument,       nullptr, 'd' },
      { nullptr,       0,                 nullptr,  0  },
    };

    while ( true ) {
      const int opt = getopt_long( argc, argv, "sSj:T:e:d", long_options, NULL );

      if ( opt == -1 ) {
        break;
      }

      switch ( opt ) {
      case 's':
        status_bar = true;
        StatusBar::get();
        break;

      case 'S':
        setenv( "GG_SANDBOXED", "1", true );
        break;

      case 'j':
        max_jobs = stoul( optarg );
        break;

      case 'T':
        timeout = stoi( optarg );
        break;

      case 'e':
      {
        string engine { optarg };
        string::size_type eqpos = engine.find( '=' );
        if ( eqpos == string::npos ) {
          engines.emplace_back( make_pair( move( engine ), move( string {} ) ) );
        }
        else {
          engines.emplace_back( make_pair( engine.substr( 0, eqpos ),
                                           engine.substr( eqpos + 1 ) ) );
        }

        break;
      }

      case 'd':
        no_download = true;
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
        if( not ThunkReader::is_thunk( target_filename ) ) {
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

    vector<unique_ptr<ExecutionEngine>> execution_engines;
    unique_ptr<StorageBackend> storage_backend;
    bool remote_execution = false;

    if ( engines.size() == 0 ) {
      /* the default engine is the local engine */
      engines.emplace_back( make_pair( "local", "" ) );
    }

    for ( const pair<string, string> & engine : engines ) {
      if ( engine.first == "local" ) {
        execution_engines.emplace_back( make_unique<LocalExecutionEngine>() );
      }
      else if ( engine.first == "lambda" ) {
        execution_engines.emplace_back( make_unique<AWSLambdaExecutionEngine>(
          AWSCredentials(), AWS::region() ) );
      }
      else if ( engine.first == "remote" ) {
        auto runner_server = gg::remote::runner_server();

        execution_engines.emplace_back( make_unique<GGExecutionEngine>(
          runner_server.first, runner_server.second ) );
      }
      else if ( engine.first == "meow" ) {
        if ( engine.second.length() == 0 ) {
          throw runtime_error( "meow: missing host public ip" );
        }

        uint16_t port = 9925;
        string::size_type colonpos = engine.second.find( ':' );
        string host_ip = engine.second.substr( 0, colonpos );

        if ( colonpos != string::npos ) {
          port = stoi( engine.second.substr( colonpos + 1 ) );
        }

        execution_engines.emplace_back( make_unique<MeowExecutionEngine>(
          AWSCredentials(), AWS::region(), Address { host_ip, port } ) );
      }
      else {
        throw runtime_error( "unknown execution engine" );
      }

      remote_execution |= execution_engines.back()->is_remote();
    }

    if ( remote_execution ) {
      storage_backend = StorageBackend::create_backend( gg::remote::storage_backends().at( 0 ) );
    }

    Reductor reductor { target_hashes, max_jobs,
                        move( execution_engines ),
                        move( storage_backend ),
                        ( timeout > 0 ) ? ( timeout * 1000 ) : -1,
                        status_bar };

    reductor.upload_dependencies();
    vector<string> reduced_hashes = reductor.reduce();
    if ( not no_download ) {
      reductor.download_targets( reduced_hashes );

      for ( size_t i = 0; i < reduced_hashes.size(); i++ ) {
        roost::copy_then_rename( gg::paths::blob_path( reduced_hashes[ i ] ), target_filenames[ i ] );

        /* HACK this is a just a dirty hack... it's not always right */
        roost::make_executable( target_filenames[ i ] );
      }
    }

    return EXIT_SUCCESS;
  }
  catch ( const exception &  e ) {
    print_exception( argv[ 0 ], e );
    return EXIT_FAILURE;
  }
}
