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
roost::path gg_reductions_path { gg_path / "reductions" };

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

roost::path get_content_path( const string & content_hash )
{
  return gg_path / content_hash;
}

string execute_thunk( const Thunk & thunk, const roost::path & thunk_path )
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

    if ( process.exit_status() != 0 ) {
      throw runtime_error( "thunk execution failed: " + thunk_path.string() );
    }
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

    if ( not process.exit_status().initialized() or process.exit_status().get() != 0 ) {
      throw runtime_error( "thunk execution failed: " + thunk_path.string() );
    }
  }

  roost::path outfile { exec_dir_path / thunk.outfile() };
  string outfile_hash = InFile::compute_hash( outfile.string() );
  roost::path outfile_gg = get_content_path( outfile_hash );

  if ( not roost::exists( outfile_gg ) ) {
    roost::move_file( outfile, outfile_gg );
  }
  else {
    roost::remove( outfile );
  }

  return outfile_hash;
}

void reduce_thunk( const roost::path &, const roost::path & thunk_path )
{
  Thunk thunk = ThunkReader( thunk_path.string() ).read_thunk();
  string thunk_hash = InFile::compute_hash( thunk_path.string() );

  if ( thunk.order() == 0 ) {
    throw runtime_error( "zero-order thunk, something is probably wrong" );
  }
  else if ( thunk.order() == 1 ) {
    string output_hash = execute_thunk( thunk, thunk_path );
    roost::path output_path = get_content_path( output_hash );
    roost::path reduction_dir = gg_reductions_path / thunk_hash / string( "0" );
    roost::create_directories( reduction_dir );
    roost::symlink( output_path, reduction_dir / output_hash );
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
      { 0, 0, 0, 0 }
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
    gg_reductions_path = gg_path / "reductions";
    roost::path thunk_path = roost::canonical( thunk_filename );

    reduce_thunk( gg_path, thunk_path );
  }
  catch ( const exception &  e ) {
    print_exception( argv[ 0 ], e );
    return EXIT_FAILURE;
  }
}
