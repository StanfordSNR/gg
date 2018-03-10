/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <unistd.h>
#include <cstring>
#include <iostream>

#include "thunk/ggutils.hh"
#include "thunk/placeholder.hh"
#include "util/exception.hh"
#include "util/path.cc"
#include "util/system_runner.hh"

using namespace std;

void usage( const char * argv0 )
{
  cerr << argv0 << "THUNK [execution args]" << endl;
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

    const string thunk_filename = argv[ 1 ];
    const roost::path thunk_path = roost::canonical( thunk_filename );

    vector<string> args = { "gg-force", thunk_filename };
    run( "gg-force", args, {}, true, true );
    CheckSystemCall( "execv", execv( thunk_path.string().c_str(), ++argv ) );
  }
  catch ( const exception &  e ) {
    print_exception( argv[ 0 ], e );
    return EXIT_FAILURE;
  }
}
