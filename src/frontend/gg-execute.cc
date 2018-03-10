/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <iostream>
#include <string>
#include <memory>
#include <sys/fcntl.h>
#include <getopt.h>
#include <vector>
#include <unordered_set>

#include "execution/execution_response.hh"
#include "net/storage_requests.hh"
#include "storage/backend.hh"
#include "thunk/ggpaths.hh"
#include "thunk/thunk_reader.hh"
#include "thunk/thunk_writer.hh"
#include "thunk/thunk.hh"
#include "util/child_process.hh"
#include "util/digest.hh"
#include "util/exception.hh"
#include "util/path.hh"
#include "util/temp_dir.hh"
#include "util/temp_file.hh"
#include "util/util.hh"

using namespace std;
using namespace gg::thunk;
using ReductionResult = gg::cache::ReductionResult;

const bool sandboxed = ( getenv( "GG_SANDBOXED" ) != NULL );
const string temp_dir_template = "/tmp/thunk-execute";
const string temp_file_template = "/tmp/thunk-file";

string execute_thunk( const Thunk & original_thunk )
{
  Thunk thunk = original_thunk;

  if ( thunk.order() != 1 ) {
    /* Let's see if we can redudce this thunk to an order one thunk by updating
    the infiles */
    const vector<InFile> new_infiles = thunk.infiles();

    for ( size_t i = 0; i < new_infiles.size(); i++ ) {
      if ( new_infiles[ i ].order() != 0 ) {
        /* let's check if we have a reduction of this infile */
        auto result = gg::cache::check( new_infiles[ i ].content_hash() );

        if ( not result.initialized() or result->order != 0 ) {
          throw runtime_error( "thunk is not order-1 and cannot be "
                               "reduced to an order-1 thunk" );
        }

        thunk.update_infile( new_infiles[ i ].content_hash(),
                             result->hash, result->order,
                             gg::hash::size( result->hash ), i );
      }
    }

    thunk.set_hash( ThunkWriter::write_thunk( thunk ) );
    cerr << "order-" << original_thunk.order() << " thunk ("
         << original_thunk.hash() << ") reduced to order-" << thunk.order()
         << " thunk (" << thunk.hash() << ")." << endl;

    assert( thunk.order() == 1 );
  }

  /* when executing the thunk, we create a temp directory, and execute the thunk
     in that directory. then we take the outfile, compute the hash, and move it
     to the .gg directory. */

  // PREPARING THE ENV
  TempDirectory exec_dir { temp_dir_template };
  roost::path exec_dir_path { exec_dir.name() };

  roost::path outfile_path { thunk.outfile() };

  if ( roost::is_absolute( outfile_path ) ) {
    throw runtime_error( "absolute path for outfiles is not supported" );
  }

  roost::path outfile_dir = roost::dirname( outfile_path );
  vector<string> path_components = outfile_dir.path_components();

  /* sometimes the outfile path can be in form of ../../../outfile. we need to
  accommodate for those cases */
  for ( const string & pc : outfile_dir.path_components() ) {
    if ( pc == ".." ) {
      exec_dir_path = exec_dir_path / "_ggsubdir";
    }
    else {
      break;
    }
  }

  // EXECUTING THE THUNK
  if ( not sandboxed ) {
    ChildProcess process {
      thunk.outfile(),
      [thunk, exec_dir_path, &outfile_dir]() {
        roost::create_directories( exec_dir_path );
        CheckSystemCall( "chdir", chdir( exec_dir_path.string().c_str() ) );
        roost::create_directories( outfile_dir );
        return thunk.execute();
      }
    };

    while ( not process.terminated() ) {
      process.wait();
    }

    if ( process.exit_status() ) {
      try {
        process.throw_exception();
      }
      catch ( const exception & ex ) {
        throw_with_nested( ExecutionError {} );
      }
    }
  }
  else {
    auto allowed_files = thunk.get_allowed_files();

    SandboxedProcess process {
      "execute(" + thunk.hash().substr( 0, 5 ) + ")",
      allowed_files,
      [thunk]() {
        return thunk.execute();
      },
      [exec_dir_path, &outfile_dir] () {
        roost::create_directories( exec_dir_path );
        CheckSystemCall( "chdir", chdir( exec_dir_path.string().c_str() ) );
        roost::create_directories( outfile_dir );
      }
    };

    try {
      process.execute();
    }
    catch( const exception & ex ) {
      throw_with_nested( ExecutionError {} );
    }
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

  // CREATING CACHE ENTRIES
  gg::cache::insert( original_thunk.hash(), outfile_hash );
  if ( original_thunk.hash() != thunk.hash() ) {
    gg::cache::insert( thunk.hash(), outfile_hash );
  }

  return outfile_hash;
}

void do_cleanup( const Thunk & thunk )
{
  unordered_set<string> infile_hashes;

  infile_hashes.emplace( thunk.hash() );
  for ( const InFile & infile : thunk.infiles() ) {
    infile_hashes.emplace( infile.content_hash() );
  }

  for ( const string & blob : roost::list_directory( gg::paths::blobs() ) ) {
    const roost::path path = gg::paths::blob_path( blob );
    if ( ( not roost::is_directory( path ) ) and infile_hashes.count( blob ) == 0 ) {
      roost::remove( path );
    }
  }
}

void fetch_dependencies( unique_ptr<StorageBackend> & storage_backend,
                         const Thunk & thunk )
{
  try {
    vector<storage::GetRequest> download_items;

    for ( const InFile & infile : thunk.infiles() ) {
      if ( infile.type() == InFile::Type::DUMMY_DIRECTORY ) continue;

      const auto target_path = gg::paths::blob_path( infile.content_hash() );

      if ( not roost::exists( target_path )
           or roost::file_size( target_path ) != infile.size() ) {
        download_items.push_back( { infile.content_hash(), target_path } );
      }
    }

    if ( download_items.size() > 0 ) {
      storage_backend->get( download_items );
    }

    for ( const InFile & infile : thunk.infiles() ) {
      if ( infile.type() == InFile::Type::EXECUTABLE ) {
        roost::make_executable( gg::paths::blob_path( infile.content_hash() ) );
      }
    }
  }
  catch ( const exception & ex ) {
    throw_with_nested( FetchDependenciesError {} );
  }
}

void upload_output( unique_ptr<StorageBackend> & storage_backend,
                    const string & output_hash )
{
  try {
    storage_backend->put( { { gg::paths::blob_path( output_hash ), output_hash,
                          digest::gghash_to_hex( output_hash ) } } );
  }
  catch ( const exception & ex ) {
    throw_with_nested( UploadOutputError {} );
  }
}

void usage( const char * argv0 )
{
  cerr << "Usage: " << argv0 << "[options] THUNK-HASH..." << endl
  << endl
  << "Options: " << endl
  << " -g, --get-dependencies  Fetch the missing dependencies from the remote storage" << endl
  << " -p, --put-output        Upload the output to the remote storage" << endl
  << " -C, --cleanup           Remove unnecessary blobs in .gg dir" << endl
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
      return to_underlying( JobStatus::OperationalFailure );
    }

    bool get_dependencies = false;
    bool put_output = false;
    bool cleanup = false;
    unique_ptr<StorageBackend> storage_backend;

    const option command_line_options[] = {
      { "get-dependencies", no_argument, nullptr, 'g' },
      { "put-output",       no_argument, nullptr, 'p' },
      { "cleanup",          no_argument, nullptr, 'C' },
      { nullptr, 0, nullptr, 0 },
    };

    while ( true ) {
      const int opt = getopt_long( argc, argv, "gpc", command_line_options, nullptr );

      if ( opt == -1 ) {
        break;
      }

      switch ( opt ) {
      case 'g': get_dependencies = true; break;
      case 'p': put_output = true; break;
      case 'C': cleanup = true; break;

      default:
        throw runtime_error( "invalid option: " + string { argv[ optind - 1 ] } );
      }
    }

    vector<string> thunk_hashes;

    for ( int i = optind; i < argc; i++ ) {
      thunk_hashes.push_back( argv[ i ] );
    }

    if ( thunk_hashes.size() == 0 ) {
      usage( argv[ 0 ] );
      return to_underlying( JobStatus::OperationalFailure );
    }

    gg::models::init();

    for ( const string & thunk_hash : thunk_hashes ) {
      /* take out an advisory lock on the thunk, in case
         other gg-execute processes are running at the same time */
      const string thunk_path = gg::paths::blob_path( thunk_hash ).string();
      FileDescriptor raw_thunk { CheckSystemCall( "open( " + thunk_path + " )",
                                                  open( thunk_path.c_str(), O_RDONLY ) ) };
      raw_thunk.block_for_exclusive_lock();

      ThunkReader thunk_reader { thunk_path };
      Thunk thunk = thunk_reader.read_thunk();

      if ( get_dependencies or put_output ) {
        storage_backend = StorageBackend::create_backend( gg::remote::storage_backend_uri() );
      }

      if ( cleanup ) {
        do_cleanup( thunk );
      }

      if ( get_dependencies ) {
        fetch_dependencies( storage_backend, thunk );
      }

      string output_hash = execute_thunk( thunk );

      if ( put_output ) {
        upload_output( storage_backend, output_hash );
      }
    }

    return to_underlying( JobStatus::Success );
  }
  catch ( const FetchDependenciesError & e ) {
    print_nested_exception( e );
    return to_underlying( JobStatus::FetchDependenciesFailure );
  }
  catch ( const ExecutionError & e ) {
    print_nested_exception( e );
    return to_underlying( JobStatus::ExecutionFailure );
  }
  catch ( const UploadOutputError & e ) {
    print_nested_exception( e );
    return to_underlying( JobStatus::UploadOutputFailure );
  }
  catch ( const exception & e ) {
    print_exception( argv[ 0 ], e );
    return to_underlying( JobStatus::OperationalFailure );
  }
}
