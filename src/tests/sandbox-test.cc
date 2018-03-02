/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <unordered_map>

#include "sandbox/sandbox.hh"
#include "util/exception.hh"

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

    const size_t total_tests = 5;
    size_t successful_tests = 0;

    /* test 1 */
    unordered_map<string, Permissions> allowed_files_1 {
      { "/dev/null", { true, true, true } }
    };

    SandboxedProcess sp_1(
      "sp_1",
      allowed_files_1,
      []()
      {
        /* XXX before, this was actually open( "/dev/null" ), but for
          some unknown reason, it was compiled to openat( -100, "/dev/null")
          in Ubuntu 17.10. Weird, right? */
        execl( "/dev/null", "/dev/null" );
        return 0;
      }
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
      "sp_2",
      allowed_files_2,
      []()
      {
        close( open( "/dev/null", O_WRONLY ) );
        close( open( "/dev/zero", O_RDONLY ) );
        return 0;
      }
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
      "sp_3",
      allowed_files_3,
      []()
      {
        struct stat buf;
        stat( "/dev/random", &buf );
        return 0;
      }
    );

    try {
      sp_3.execute();
    }
    catch (...) {
      successful_tests++;
    }

    /* test 4 */
    unordered_map<string, Permissions> allowed_files_4 {
      { "/bin/blahblah", { true, false, true } },
    };

    SandboxedProcess sp_4(
      "sp_4",
      allowed_files_4,
      []()
      {
        execl( "/bin/blahblah", "/bin/blahblah" );
        return 0;
      }
    );

    try {
      sp_4.execute();
      successful_tests++;
    }
    catch (...) {
    }

    /* test 5 */
    unordered_map<string, Permissions> allowed_files_5 {
      { "/bin/ls", { true, false, false } },
    };

    SandboxedProcess sp_5(
      "sp_5",
      allowed_files_5,
      []()
      {
        execl( "/bin/ls", "/bin/ls" );
        return 0;
      }
    );

    try {
      sp_5.execute();
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
