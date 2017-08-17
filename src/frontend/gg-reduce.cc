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
#include "temp_file.hh"
#include "temp_dir.hh"
#include "thunk_writer.hh"
#include "ggpaths.hh"
#include "placeholder.hh"
#include "system_runner.hh"

using namespace std;
using namespace gg::thunk;

const LogLevel log_level = LOG_LEVEL_NO_LOG;

const bool sandboxed = ( getenv( "GG_SANDBOXED" ) != NULL );
const string temp_dir_template = "/tmp/thunk-execute";
const string temp_file_template = "/tmp/thunk-file";

const roost::path gg_path = gg::paths::blobs();
const roost::path gg_reductions_path = gg::paths::reductions();

struct ReductionResult
{
  string hash;
  size_t order;
};

Optional<ReductionResult> check_reduction_cache( const string & thunk_hash )
{
  roost::path reduction { gg_reductions_path / thunk_hash };

  if ( not roost::lexists( reduction ) ) {
    return {}; // no reductions are available
  }

  return ReductionResult { roost::readlink( reduction ), 0 };
}

string execute_thunk( const string & thunk_hash )
{
  ChildProcess execute_process { thunk_hash,
    [thunk_hash]()
    {
      vector<string> command { "gg-execute", thunk_hash };
      return ezexec( command[ 0 ], command, {}, true, true );
    }
  };

  while ( not execute_process.terminated() ) { execute_process.wait(); }

  if ( execute_process.exit_status() != 0 ) {
    throw runtime_error( "thunk execution failed: " + thunk_hash );
  }

  Optional<ReductionResult> result = check_reduction_cache( thunk_hash );

  if ( not result.initialized() or result->order != 0 ) {
    throw runtime_error( "could not find the reduction entry" );
  }

  return result->hash;

}

/* Reduces the order of the input thunk by one and returns hash of the reduction
   result */
ReductionResult recursive_reduce( const string & thunk_hash )
{
  ThunkReader thunk_reader { gg::paths::blob_path( thunk_hash ).string() };

  if ( not thunk_reader.is_thunk() ) {
    /* already reduced. gg's work is done here. */
    return { "", 0 };
  }

  Thunk thunk = thunk_reader.read_thunk();

  /* first let's see if we actually have a reduced version of this thunk */
  const auto cached_reduction = check_reduction_cache( thunk_hash );

  if ( cached_reduction.initialized() ) {
    return *cached_reduction;
  }

  if ( thunk.order() == 0 ) {
    throw runtime_error( "zero-order thunk, something is probably wrong" );
  }
  else if ( thunk.order() == 1 ) {
    const string output_hash = execute_thunk( thunk_hash );
    return { output_hash, 0 };
  }
  else { // thunk.order() >= 2
    vector<InFile> new_infiles;

    for ( const InFile & infile : thunk.infiles() ) {
      if ( infile.order() == thunk.order() - 1 )  {
        const ReductionResult reduction = recursive_reduce( infile.content_hash() );
        const roost::path reduction_path = gg::paths::blob_path( reduction.hash );
        const off_t reduction_size = roost::file_size( reduction_path );

        new_infiles.emplace_back( infile.filename(), reduction_path.string(),
                                  reduction.hash, reduction.order, reduction_size );
      }
      else {
        new_infiles.push_back( infile );
      }
    }

    Thunk new_thunk { thunk.outfile(), thunk.function(), new_infiles };
    return { ThunkWriter::write_thunk( new_thunk ), new_thunk.order() };
  }
}

void usage( const char * argv0 )
{
  cerr << argv0 << " THUNK [execution args]" << endl
       << endl
       << "Useful environment variables:" << endl
       << "  GG_DIR       => absolute path to gg directory" << endl
       << "  GG_SANDBOXED => if set, forces the thunks in a sandbox" << endl
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

    string thunk_filename { argv[ 1 ] };
    const roost::path thunk_path = roost::canonical( thunk_filename );

    /* first check if this file is actually a placeholder */
    Optional<ThunkPlaceholder> placeholder = ThunkPlaceholder::read( thunk_path.string() );

    if ( placeholder.initialized() ) {
      copy_then_rename( gg_path / placeholder->content_hash(), thunk_path );
    }

    string final_hash = InFile::compute_hash( thunk_path.string() );
    string reduced_hash = recursive_reduce( final_hash ).hash;

    while ( not reduced_hash.empty() ) {
      final_hash = reduced_hash;
      reduced_hash = recursive_reduce( reduced_hash ).hash;
    }

    roost::copy_then_rename( gg::paths::blob_path( final_hash ), thunk_path );

    return EXIT_SUCCESS;
  }
  catch ( const exception &  e ) {
    print_exception( argv[ 0 ], e );
    return EXIT_FAILURE;
  }
}
