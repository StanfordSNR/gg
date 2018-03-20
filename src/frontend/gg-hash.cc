/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <iostream>
#include <string>
#include <cstdlib>
#include <fcntl.h>

#include "thunk/factory.hh"
#include "thunk/ggutils.hh"
#include "thunk/thunk_reader.hh"
#include "thunk/thunk.hh"
#include "util/exception.hh"
#include "util/file_descriptor.hh"

using namespace std;
using namespace gg;

void usage( const char * argv0 )
{
  cerr << argv0 << " FILENAME" << endl;
}

int main( int argc, char * argv[] )
{
  try {
    if ( argc <= 0 ) {
      abort();
    }

    if ( argc != 2 ) {
      usage( argv[ 0 ] );
      return EXIT_FAILURE;
    }

    const string filename { argv[ 1 ] };
    const ObjectType type = ThunkReader( filename ).is_thunk() ? ObjectType::Thunk
                                                               : ObjectType::Value;

    FileDescriptor file { CheckSystemCall( "open (" + filename + ")",
                                           open( filename.c_str(), O_RDONLY ) ) };

    string contents;
    while ( not file.eof() ) { contents += file.read(); }
    cout << gg::hash::compute( contents, type ) << endl;
  }
  catch ( const exception &  e ) {
    print_exception( argv[ 0 ], e );
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
