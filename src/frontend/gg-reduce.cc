/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <unistd.h>
#include <cstring>
#include <getopt.h>
#include <iostream>
#include <unordered_set>

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

ReductionResult reduce_thunk( const string & thunk_hash )
{
  DependencyGraph dep_graph;
  dep_graph.add_thunk( thunk_hash );

  unordered_set<string> order_one_deps = dep_graph.order_one_dependencies( thunk_hash );

  while ( not order_one_deps.empty() ) {
    string dep_hash = *order_one_deps.begin();
    string output_hash;
    Optional<ReductionResult> cached = check_reduction_cache( dep_hash );

    if ( not cached.initialized() ) {
      output_hash = execute_thunk( dep_hash );
    }
    else {
      output_hash = cached->hash;
    }

    unordered_set<string> new_o1s = dep_graph.force_thunk( dep_hash, output_hash );
    order_one_deps.insert( new_o1s.begin(), new_o1s.end() );
    order_one_deps.erase( dep_hash );
  }

  return *check_reduction_cache( dep_graph.updated_hash( thunk_hash ) );
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

    string thunk_hash = InFile::compute_hash( thunk_path.string() );
    string reduced_hash = reduce_thunk( thunk_hash ).hash;

    roost::copy_then_rename( gg::paths::blob_path( reduced_hash ), thunk_path );

    return EXIT_SUCCESS;
  }
  catch ( const exception &  e ) {
    print_exception( argv[ 0 ], e );
    return EXIT_FAILURE;
  }
}
