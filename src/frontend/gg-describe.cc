/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <unistd.h>
#include <cstring>
#include <iostream>
#include <google/protobuf/text_format.h>

#include "exception.hh"
#include "syscall.hh"
#include "thunk.hh"
#include "thunk_reader.hh"

using namespace std;
using namespace google::protobuf;

void usage( const char * argv0 )
{
  cerr << argv0 << " THUNK" << endl;
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

    Thunk thunk = ThunkReader::read_thunk( argv[ 1 ] );

    string textf;
    TextFormat::PrintToString( thunk.to_protobuf(), &textf );

    cout << textf << endl;
  }
  catch ( const exception &  e ) {
    print_exception( argv[ 0 ], e );
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
