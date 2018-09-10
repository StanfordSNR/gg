/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <iostream>

#include "thunk/ggutils.hh"
#include "util/exception.hh"
#include "util/path.hh"

using namespace std;
using namespace gg;

void usage( const char * argv0 )
{
  cerr << argv0 << " FILE..." << endl;
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

    gg::paths::blobs(); // Trigger the exception if GG_DIR is not set.

    for ( int i = 1; i < argc; i++ ) {
      roost::path src { argv[ i ] };
      string hash = gg::hash::file( src );
      roost::path dst = gg::paths::blob( hash );

      if ( not roost::exists( dst ) ) {
        const mode_t permission = roost::is_executable( src ) ? 0500 : 0400;
        roost::copy_then_rename( src, dst, true, permission );
      }

      cout << hash << endl;
    }

    return EXIT_SUCCESS;
  }
  catch ( const exception &  e ) {
    print_exception( argv[ 0 ], e );
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
