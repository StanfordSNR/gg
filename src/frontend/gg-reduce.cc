/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <unistd.h>
#include <cstring>
#include <getopt.h>
#include <iostream>

#include "exception.hh"
#include "thunk.hh"
#include "thunk_reader.hh"
#include "child_process.hh"
#include "sandbox.hh"
#include "path.hh"
#include "temp_dir.hh"

using namespace std;
using namespace gg::thunk;

bool sandboxed = false;
LogLevel log_level = LOG_LEVEL_NO_LOG;
string temp_root = "/tmp/thunk-execute";
roost::path gg_path { ".gg" };

void usage( const char * argv0 )
{
  cerr << argv0 << "[--verbose, -v] [--sandboxed,-s] [--gg-dir, -g=<arg>] THUNK" << endl;
}

inline void CheckExecution( const string & path, bool status )
{
  if ( not status ) {
    throw runtime_error( "thunk execution failed: " +  path );
  }
}

bool execute_thunk( const Thunk & thunk, const roost::path & thunk_path )
{
  assert( thunk.order() == 1 );

  /* when executing the thunk, we create a temp directory, and execute the thunk
     in that directory. then we take the outfile, compute the hash, and move it
     to the .gg directory. */

  TempDirectory exec_dir { temp_root };
  roost::path exec_dir_path { exec_dir.name() };

  if ( not sandboxed ) {
    ChildProcess process {
      thunk.outfile(),
      [thunk, thunk_path, exec_dir_path]() {
        CheckSystemCall( "chdir", chdir( exec_dir_path.string().c_str() ) );
        return thunk.execute( gg_path, thunk_path );
      }
    };

    while ( not process.terminated() ) {
      process.wait();
    }

    return process.exit_status() == 0;
  }
  else {
    auto allowed_files = thunk.get_allowed_files( gg_path, thunk_path );

    SandboxedProcess process {
      allowed_files,
      [thunk, thunk_path]() {
        return thunk.execute( gg_path, thunk_path );
      },
      [exec_dir_path] () {
        CheckSystemCall( "chdir", chdir( exec_dir_path.string().c_str() ) );
      }
    };

    process.set_log_level( log_level );
    process.execute();

    return ( process.exit_status().initialized() and process.exit_status().get() == 0 );
  }
}

void reduce_thunk( const roost::path &, const roost::path & thunk_path )
{
  Thunk thunk = ThunkReader( thunk_path.string() ).read_thunk();

  if ( thunk.order() == 0 ) {
    throw runtime_error( "zero-order thunk, something is probably wrong" );
  }
  else if ( thunk.order() == 1 ) {
    CheckExecution( thunk_path.string(), execute_thunk( thunk, thunk_path ) );
  }
  else {
    throw runtime_error( "order > 1, not implemented." );
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

    string gg_dir = ".gg/";

    const option command_line_options[] = {
      { "sandboxed", no_argument,       nullptr, 's' },
      { "gg-dir",    required_argument, nullptr, 'g' },
      { "verbose",   no_argument,       nullptr, 'v' },
    };

    while ( true ) {
      const int opt = getopt_long( argc, argv, "vsg:", command_line_options, nullptr );

      if ( opt == -1 ) {
        break;
      }

      switch ( opt ) {
      case 's': sandboxed = true; break;
      case 'g': gg_dir = optarg; break;
      case 'v': log_level = LOG_LEVEL_DEBUG; break;
      default:
        throw runtime_error( "invalid option" );
      }
    }

    if ( optind >= argc ) {
      usage( argv[ 0 ] );
      return EXIT_FAILURE;
    }

    string thunk_filename = argv[ optind ];

    gg_path = roost::canonical( gg_dir );
    roost::path thunk_path = roost::canonical( thunk_filename );

    reduce_thunk( gg_path, thunk_path );
  }
  catch ( const exception &  e ) {
    print_exception( argv[ 0 ], e );
    return EXIT_FAILURE;
  }
}
