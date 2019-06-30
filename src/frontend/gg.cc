/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <iostream>
#include <string>

#include "util/exception.hh"
#include "util/system_runner.hh"

using namespace std;

void usage( const char * argv0 )
{
  cerr << argv0 << " COMMAND [ARG]..." << endl;
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

    const string command = string( "gg-" ) + argv[ 1 ];
    vector<string> args { command };

    for ( int i = 2; i < argc; i++ ) {
        args.emplace_back( argv[ i ] );
    }

    ezexec( command, args, {}, true, true );

    cerr << "gg: '" << argv[ 1 ] << "' is not a gg command." << endl;
    return EXIT_FAILURE;
  }
  catch ( const exception &  e ) {
    print_exception( argv[ 0 ], e );
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
