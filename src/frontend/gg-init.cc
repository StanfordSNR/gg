/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <iostream>

#include "util/exception.hh"
#include "util/path.hh"

using namespace std;

void usage( const char * argv0 )
{
  cerr << argv0 << " [directory]" << endl;
}

int main( int argc, char * argv[] )
{
  try {
    if ( argc <= 0 ) {
      abort();
    }

    if ( argc > 2 ) {
      usage( argv[ 0 ] );
      return EXIT_FAILURE;
    }

    roost::path gg_dir = ( argc == 1 ) ? ( roost::current_working_directory() / ".gg" )
                                       : argv[ 1 ];

    roost::create_directories( gg_dir );
    cerr << "Initialized gg directory at: " << gg_dir.string() << endl;

    return EXIT_SUCCESS;
  }
  catch ( const exception &  e ) {
    print_exception( argv[ 0 ], e );
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
