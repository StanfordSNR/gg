/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <iostream>
#include <string>
#include <sys/fcntl.h>

#include "exception.hh"
#include "thunk.hh"
#include "path.hh"
#include "ggpaths.hh"
#include "child_process.hh"
#include "temp_file.hh"
#include "temp_dir.hh"
#include "thunk_reader.hh"

using namespace std;
using namespace gg::thunk;
using ReductionResult = gg::cache::ReductionResult;

const bool sandboxed = ( getenv( "GG_SANDBOXED" ) != NULL );
const string temp_dir_template = "/tmp/thunk-execute";
const string temp_file_template = "/tmp/thunk-file";

string execute_thunk( const Thunk & thunk, const std::string & thunk_hash )
{
  if ( thunk.order() != 1 ) {
    throw runtime_error( "thunk is not executable (order != 1)" );
  }

  gg::models::init();

  /* when executing the thunk, we create a temp directory, and execute the thunk
     in that directory. then we take the outfile, compute the hash, and move it
     to the .gg directory. */

  // PREPARING THE ENV
  TempDirectory exec_dir { temp_dir_template };
  roost::path exec_dir_path { exec_dir.name() };

  roost::path outfile_dir = roost::dirname( thunk.outfile() );

  // EXECUTING THE THUNK
  if ( not sandboxed ) {
    ChildProcess process {
      thunk.outfile(),
      [thunk, thunk_hash, exec_dir_path, &outfile_dir]() {
        CheckSystemCall( "chdir", chdir( exec_dir_path.string().c_str() ) );
        roost::create_directories( outfile_dir );
        return thunk.execute( thunk_hash );
      }
    };

    while ( not process.terminated() ) {
      process.wait();
    }

    if ( process.exit_status() != 0 ) {
      throw runtime_error( "thunk execution failed: " + thunk_hash );
    }
  }
  else {
    auto allowed_files = thunk.get_allowed_files( thunk_hash );

    SandboxedProcess process {
      "execute(" + thunk_hash.substr( 0, 5 ) + ")",
      allowed_files,
      [thunk, thunk_hash]() {
        return thunk.execute( thunk_hash );
      },
      [exec_dir_path, &outfile_dir] () {
        CheckSystemCall( "chdir", chdir( exec_dir_path.string().c_str() ) );
        roost::create_directories( outfile_dir );
      }
    };

    process.execute();
  }

  // GRABBING THE OUTPUT
  roost::path outfile { exec_dir_path / thunk.outfile() };
  string outfile_hash = InFile::compute_hash( outfile.string() );
  roost::path outfile_gg = gg::paths::blob_path( outfile_hash );

  if ( not roost::exists( outfile_gg ) ) {
    roost::move_file( outfile, outfile_gg );
  } else {
    roost::remove( outfile );
  }

  return outfile_hash;
}

void usage( const char * argv0 )
{
  cerr << argv0 << " THUNK-HASH" << endl;
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

    string thunk_hash { argv[ 1 ] };

    /* take out an advisory lock on the thunk, in case
       other gg-execute processes are running at the same time */
    const string thunk_path = gg::paths::blob_path( thunk_hash ).string();
    FileDescriptor raw_thunk { CheckSystemCall( "open( " + thunk_path + " )",
                                                open( thunk_path.c_str(), O_RDONLY ) ) };
    raw_thunk.block_for_exclusive_lock();

    if ( gg::cache::check( thunk_hash ).initialized() ) {
      /* already reduced */
      return EXIT_SUCCESS;
    }

    ThunkReader thunk_reader { thunk_path };
    Thunk thunk = thunk_reader.read_thunk();
    string output_hash = execute_thunk( thunk, thunk_hash );
    gg::cache::insert( thunk_hash, output_hash );

    return EXIT_SUCCESS;
  }
  catch ( const exception &  e ) {
    print_exception( argv[ 0 ], e );
    return EXIT_FAILURE;
  }
}
