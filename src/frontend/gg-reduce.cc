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

using namespace std;
using namespace gg::thunk;

bool sandboxed = false;
LogLevel log_level = LOG_LEVEL_NO_LOG;
const string temp_dir_template = "/tmp/thunk-execute";
const string temp_file_template = "/tmp/thunk-file";

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

  TempDirectory exec_dir { temp_dir_template };
  roost::path exec_dir_path { exec_dir.name() };

  roost::path outfile_dir = roost::dirname( thunk.outfile() );
  roost::create_directories( outfile_dir );

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

  roost::remove( outfile );

  return outfile_hash;
}

void store_thunk_reduction( const string & original_hash,
                            const string & new_hash,
                            const size_t & new_order )
{
  const roost::path new_path = get_content_path( new_hash );
  const roost::path reduction_dir = gg_reductions_path / original_hash / to_string( new_order );
  const roost::path symlink_target = reduction_dir / new_hash;

  if ( not roost::exists( symlink_target ) ) {
    roost::create_directories( reduction_dir );
    roost::symlink( new_path, reduction_dir / new_hash );
  }
}

string check_reduction_cache( const string & thunk_hash, const size_t order )
{
  if ( order == 0 ) {
    throw runtime_error( "not a thunk" );
  }

  roost::path reduction_root { gg_reductions_path / thunk_hash };

  if ( not roost::exists( reduction_root ) ) {
    return ""; // no reductions are available
  }

  for ( size_t i = 0; i < order; i++ ) {
    roost::path reduction_order_root { reduction_root / to_string( i ) };

    if ( not roost::exists( reduction_order_root ) ) {
      continue;
    }

    const vector<string> reductions = roost::get_directory_listing( reduction_order_root );

    if ( reductions.size() > 0 ) {
      return reductions.front();
    }
  }

  return "";
}

/* Reduces the order of the input thunk by one and returns hash of the reduction
   result */
string reduce_thunk( const roost::path & gg_path, const roost::path & thunk_path )
{
  ThunkReader thunk_reader { thunk_path.string() };

  if ( not thunk_reader.is_thunk() ) {
    /* already reduced. gg's work is done here. */
    return "";
  }

  Thunk thunk = thunk_reader.read_thunk();
  const string thunk_hash = InFile::compute_hash( thunk_path.string() );

  cerr << "Reducing [" << thunk_hash << "](" << thunk.order() << ")" << endl;

  /* first let's see if we actually have a reduced version of this thunk */
  const string cached_reduction = check_reduction_cache( thunk_hash, thunk.order() );

  if ( not cached_reduction.empty() ) {
    cerr << "Reduction found: " << cached_reduction << endl;
    return cached_reduction;
  }

  if ( thunk.order() == 0 ) {
    throw runtime_error( "zero-order thunk, something is probably wrong" );
  }
  else if ( thunk.order() == 1 ) {
    const string output_hash = execute_thunk( thunk, thunk_path );
    store_thunk_reduction( thunk_hash, output_hash, 0 );

    cerr << "Reduced [" << thunk_hash << "](" << thunk.order() << ") => "
         << "[" << output_hash << "](" << 0 << ")" << endl;

    return output_hash;
  }
  else { // thunk.order() >= 2
    vector<InFile> new_infiles;

    for ( const InFile & infile : thunk.infiles() ) {
      if ( infile.order() == thunk.order() - 1 )  {
        const string reduced_hash = reduce_thunk( gg_path, gg_path / infile.content_hash() );
        const roost::path reduced_path = get_content_path( reduced_hash );

        new_infiles.emplace_back( infile.filename(), reduced_path.string(),
                                  reduced_hash );
      }
      else {
        new_infiles.push_back( infile );
      }
    }

    Thunk new_thunk { thunk.outfile(), thunk.function(), new_infiles };

    TempFile temp_thunk { temp_file_template };
    ThunkWriter::write_thunk( new_thunk, temp_thunk.name() );
    const string new_thunk_hash = InFile::compute_hash( temp_thunk.name() );
    const roost::path new_thunk_path = get_content_path( new_thunk_hash );

    if ( not roost::exists( new_thunk_path ) ) {
      roost::move_file( temp_thunk.name(), new_thunk_path );
    }

    store_thunk_reduction( thunk_hash, new_thunk_hash, new_thunk.order() );

    cerr << "Reduced [" << thunk_hash << "](" << thunk.order() << ") => "
         << "[" << new_thunk_hash << "](" << new_thunk.order() << ")" << endl;

    return new_thunk_hash;
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
    const roost::path thunk_path = roost::canonical( thunk_filename );

    string final_hash = InFile::compute_hash( thunk_path.string() );

    string reduced_hash = reduce_thunk( gg_path, thunk_path );

    while ( not reduced_hash.empty() ) {
      final_hash = reduced_hash;
      cerr << "Reduced to " << reduced_hash << endl;
      reduced_hash = reduce_thunk( gg_path, get_content_path( reduced_hash ) );
    }

    cerr << "Final hash: " << final_hash << endl;
    cerr << "Putting the outfile... ";

    roost::copy_then_rename( get_content_path( final_hash ), thunk_path.string() );
    
    cerr << "done" << endl;

    return EXIT_SUCCESS;
  }
  catch ( const exception &  e ) {
    print_exception( argv[ 0 ], e );
    return EXIT_FAILURE;
  }
}
