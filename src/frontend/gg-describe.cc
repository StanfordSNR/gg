/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <unistd.h>
#include <cstring>
#include <iostream>
#include <getopt.h>
#include <google/protobuf/util/json_util.h>

#include "thunk/thunk.hh"
#include "thunk/thunk_reader.hh"
#include "thunk/ggutils.hh"
#include "trace/syscall.hh"
#include "util/exception.hh"

using namespace std;
using namespace google::protobuf::util;
using namespace gg::thunk;

void usage( const char * argv0 )
{
  cerr << argv0 << " [--executable-hash, -e] THUNK-HASH" << endl;
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

    const option command_line_options[] = {
      { "executable-hash", no_argument, nullptr, 'e' },
      { 0, 0, 0, 0 }
    };

    while ( true ) {
      const int opt = getopt_long( argc, argv, "e", command_line_options, nullptr );

      if ( opt == -1 ) { break; }

      switch ( opt ) {
      case 'e':
        print_executable_hash = true;
        break;
      }
    }

    if ( optind >= argc ) {
      usage( argv[ 0 ] );
      return EXIT_FAILURE;
    }

    roost::path thunk_path { gg::paths::blob_path( argv[ optind ] ) };

    if ( not roost::exists( thunk_path ) ) {
      thunk_path = argv[ optind ];
    }

    ThunkReader thunk_reader { thunk_path.string() };
    Thunk thunk = thunk_reader.read_thunk();

    if ( print_executable_hash ) {
      cout << thunk.executable_hash() << endl;
    }
    else {
      string textf;
      JsonPrintOptions options;
      options.add_whitespace = true;
      options.always_print_primitive_fields = true;
      MessageToJsonString( thunk.to_protobuf(), &textf, options );

      cout << textf << endl;
    }
  }
  catch ( const exception &  e ) {
    print_exception( argv[ 0 ], e );
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
