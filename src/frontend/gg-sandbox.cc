/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <unordered_map>

#include "exception.hh"
#include "sandbox.hh"

using namespace std;

void usage( const char * argv0 )
{
  cerr << argv0 << endl;
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

    unordered_map<string, Permissions> allowed_files {
      { "/dev/null", { false, true, true } }
    };

    SandboxedProcess sp(
      []()
      {
        return open( "/dev/null", O_RDONLY );
      },
      allowed_files
    );

    sp.execute();
  }
  catch ( const exception &  e ) {
    print_exception( argv[ 0 ], e );
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
