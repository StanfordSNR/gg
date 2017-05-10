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

using namespace std;
using namespace gg::thunk;

void usage( const char * argv0 )
{
  cerr << argv0 << "[--sandboxed,-s] [--gg-dir, -g=<arg>] THUNK" << endl;
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

    bool sandboxed = false;
    string gg_dir = ".gg/";

    const option command_line_options[] = {
      { "sandboxed", no_argument,       nullptr, 's' },
      { "gg-dir",    required_argument, nullptr, 'g' },
    };

    while ( true ) {
      const int opt = getopt_long( argc, argv, "sg:", command_line_options, nullptr );

      if ( opt == -1 ) {
        break;
      }

      switch ( opt ) {
      case 's': sandboxed = true; break;
      case 'g': gg_dir = optarg; break;
      default:
        throw runtime_error( "invalid option" );
      }
    }

    if ( optind >= argc ) {
      usage( argv[ 0 ] );
      return EXIT_FAILURE;
    }

    ThunkReader thunk_reader { argv[ optind ] };
    Thunk thunk = thunk_reader.read_thunk();

    if ( not sandboxed ) {
      ChildProcess process { thunk.outfile(), [&]() { return thunk.execute(); } };

      while ( not process.terminated() ) {
        process.wait();
      }

      cerr << "Exited: " << process.exit_status() << endl;
    }
    else {
      unordered_map<string, Permissions> allowed_files;

      for ( const InFile & infile : thunk.infiles() ) {
        allowed_files[ gg_dir.append( infile.hash() ) ] = { true, true, true };
      }

      SandboxedProcess process { [&]() { return thunk.execute(); }, allowed_files };
      process.execute();
    }
  }
  catch ( const exception &  e ) {
    print_exception( argv[ 0 ], e );
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
