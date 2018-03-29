/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <iostream>
#include <stdexcept>
#include <cstdlib>

using namespace std;

void usage( char * argv0 )
{
  cerr << "Usage: " << argv0 << endl;
}

int main( int argc, char * argv[] )
{
  try {
    if ( argc <= 0 ) {
      abort();
    }

    if ( argc != 1 ) {
      usage( argv[ 0 ] );
      return EXIT_FAILURE;
    }
  }
  catch ( const exception & e ) {
    return EXIT_FAILURE;
  }
}
