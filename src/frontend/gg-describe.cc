/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <unistd.h>
#include <cstring>
#include <iostream>
#include <google/protobuf/util/json_util.h>

#include "exception.hh"
#include "syscall.hh"
#include "thunk.hh"
#include "thunk_reader.hh"

using namespace std;
using namespace google::protobuf::util;
using namespace gg::thunk;

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

    ThunkReader thunk_reader { argv[ 1 ] };
    Thunk thunk = thunk_reader.read_thunk();

    string textf;
    JsonPrintOptions options;
    options.add_whitespace = true;
    options.always_print_primitive_fields = true;
    MessageToJsonString( thunk.to_protobuf(), &textf, options );

    cout << "// [order=" << thunk.order() << "]" << endl;
    cout << textf << endl;
  }
  catch ( const exception &  e ) {
    print_exception( argv[ 0 ], e );
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
