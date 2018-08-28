/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <unistd.h>
#include <cstring>
#include <iostream>
#include <getopt.h>
#include <glob.h>

#include "protobufs/util.hh"
#include "thunk/thunk.hh"
#include "thunk/thunk_reader.hh"
#include "thunk/ggutils.hh"
#include "trace/syscall.hh"
#include "util/exception.hh"
#include "util/system_runner.hh"

using namespace std;
using namespace google::protobuf::util;
using namespace gg::thunk;

void usage( const char * argv0 )
{
  cerr << "Usage: " << argv0 << endl
       << "       " << "[-e|--executable-hash] [-c|--command]" << endl
       << "       " << "THUNK-HASH" << endl;
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

    bool print_executable_hash = false;
    bool print_command = false;

    const option command_line_options[] = {
      { "executable-hash", no_argument, nullptr, 'e' },
      { "command",         no_argument, nullptr, 'c' },
      { 0, 0, 0, 0 }
    };

    while ( true ) {
      const int opt = getopt_long( argc, argv, "ec", command_line_options, nullptr );

      if ( opt == -1 ) { break; }

      switch ( opt ) {
      case 'e':
        print_executable_hash = true;
        break;

      case 'c':
        print_command = true;
        break;
      }
    }

    if ( optind >= argc ) {
      usage( argv[ 0 ] );
      return EXIT_FAILURE;
    }

    roost::path thunk_path { argv[ optind ] };

    if ( not roost::exists( thunk_path ) ) {
      thunk_path = gg::paths::blob( argv[ optind ] );

      if ( not roost::exists( thunk_path ) ) {
        roost::path pattern { thunk_path.string() + "*" };

        glob_t glob_result;

        if ( glob( pattern.string().c_str(), GLOB_ERR | GLOB_NOSORT,
                      nullptr, &glob_result ) == 0 ) {
          if ( glob_result.gl_pathc > 1 ) {
            cerr << "Partial hash, multiple matches found." << endl;
            return EXIT_FAILURE;
          }
          else if ( glob_result.gl_pathc == 1 ) {
            thunk_path = roost::path { glob_result.gl_pathv[ 0 ] };
            cerr << thunk_path.string() << endl;
          }
        }

        globfree( &glob_result );
      }
    }

    Thunk thunk = ThunkReader::read( thunk_path );

    if ( print_executable_hash ) {
      cout << thunk.executable_hash() << endl;
    }

    if ( print_command ) {
      cout << command_str( thunk.function().args(), thunk.function().envars() ) << endl;
    }

    if ( not print_executable_hash and not print_command ) {
      cout << protoutil::to_json( thunk.to_protobuf(), true ) << endl;
    }
  }
  catch ( const exception &  e ) {
    print_exception( argv[ 0 ], e );
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
