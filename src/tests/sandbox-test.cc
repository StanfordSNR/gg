/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
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

    const size_t total_tests = 3;
    size_t successful_tests = 0;

    /* test 1 */
    unordered_map<string, Permissions> allowed_files_1 {
      { "/dev/null", { true, true, true } }
    };

    SandboxedProcess sp_1(
      []()
      {
        close( open( "/dev/null", O_RDONLY ) );
        return 0;
      },
      allowed_files_1
    );

    try {
      sp_1.execute();
      successful_tests++;
    }
    catch (...) {}

    /* test 2 */
    unordered_map<string, Permissions> allowed_files_2 {
      { "/dev/null", { true, true, true } },
      { "/dev/zero", { false, true, true } },
    };

    SandboxedProcess sp_2(
      []()
      {
        close( open( "/dev/null", O_WRONLY ) );
        close( open( "/dev/zero", O_RDONLY ) );
        return 0;
      },
      allowed_files_2
    );

    try {
      sp_2.execute();
    }
    catch (...) {
      successful_tests++;
    }

    /* test 3 */
    unordered_map<string, Permissions> allowed_files_3 {
      { "/dev/zero", { true, true, true } },
      { "/dev/null", { true, true, true } },
    };

    SandboxedProcess sp_3(
      []()
      {
        struct stat buf;
        stat( "/dev/random", &buf );
        return 0;
      },
      allowed_files_3
    );

    try {
      sp_3.execute();
    }
    catch (...) {
      successful_tests++;
    }

    return successful_tests != total_tests;
  }
  catch ( const exception &  e ) {
    print_exception( argv[ 0 ], e );
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
