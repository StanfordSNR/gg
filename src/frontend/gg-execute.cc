/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <iostream>
#include <string>

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

const roost::path gg_path = gg::paths::blobs();
const roost::path gg_reductions_path = gg::paths::reductions();

const bool sandboxed = ( getenv( "GG_SANDBOXED" ) != NULL );
const string temp_dir_template = "/tmp/thunk-execute";
const string temp_file_template = "/tmp/thunk-file";

void store_thunk_reduction( const string & original_hash,
                            const string & new_hash )
{
  roost::symlink( new_hash, gg_reductions_path / original_hash );
}

Optional<string> check_reduction_cache( const string & thunk_hash )
{
  roost::path reduction { gg_reductions_path / thunk_hash };

  if ( not roost::lexists( reduction ) ) {
    return {}; // no reductions are available
  }

  return roost::readlink( reduction );
}

string execute_thunk( const Thunk & thunk, const roost::path & thunk_path )
{
  if ( thunk.order() != 1 ) {
    throw runtime_error( "thunk is not executable (order != 1)" );
  }

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
      [thunk, thunk_path, exec_dir_path, &outfile_dir]() {
        CheckSystemCall( "chdir", chdir( exec_dir_path.string().c_str() ) );
        roost::create_directories( outfile_dir );
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
      [exec_dir_path, &outfile_dir] () {
        CheckSystemCall( "chdir", chdir( exec_dir_path.string().c_str() ) );
        roost::create_directories( outfile_dir );
      }
    };

    process.execute();

    if ( not process.exit_status().initialized() or process.exit_status().get() != 0 ) {
      throw runtime_error( "thunk execution failed: " + thunk_path.string() );
    }
  }

  // GRABBING THE OUTPUT
  roost::path outfile { exec_dir_path / thunk.outfile() };
  string outfile_hash = InFile::compute_hash( outfile.string() );
  roost::path outfile_gg = gg::paths::blob_path( outfile_hash );

  if ( not roost::exists( outfile_gg ) ) {
    roost::move_file( outfile, outfile_gg );
  }

  roost::remove( outfile );

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

    if ( check_reduction_cache( thunk_hash ).initialized() ) {
      /* already reduced */
      return EXIT_SUCCESS;
    }

    roost::path thunk_path = gg::paths::blob_path( thunk_hash );
    ThunkReader thunk_reader { thunk_path.string() };
    Thunk thunk = thunk_reader.read_thunk();
    string output_hash = execute_thunk( thunk, thunk_path );
    store_thunk_reduction( thunk_hash, output_hash );

    return EXIT_SUCCESS;
  }
  catch ( const exception &  e ) {
    print_exception( argv[ 0 ], e );
    return EXIT_FAILURE;
  }
}
