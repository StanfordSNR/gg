/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <libgen.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <cstring>
#include <unordered_map>

#include "traced_process.hh"
#include "exception.hh"
#include "placeholder.hh"
#include "file_descriptor.hh"

using namespace std;

void usage( const char * argv0 )
{
  cerr << argv0 << " COMMAND..." << endl;
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

    TracedProcess tp( [&argv]() { return execvp( argv[ 1 ], &argv[ 1 ] ); } );

    while ( true ) {
      int waitres = tp.wait_for_syscall(
        [&]( TraceControlBlock & tcb )
        {
          Optional<SystemCallInvocation> & invocation = tcb.syscall_invocation;

          switch ( invocation->syscall_no() ) {
          case SYS_ioctl:
            invocation->fetch_arguments();
            if ( invocation->arguments()->at( 0 ).value<int>() == 0 and
                 invocation->arguments()->at( 1 ).value<int>() == 0x03031990 ) {
              tcb.detach();
            }

            break;

          case SYS_open:
            {
              invocation->fetch_arguments();
              string open_path = invocation->arguments()->at( 0 ).value<string>();

              int fd_num = open( open_path.c_str(), O_RDONLY );

              if ( fd_num < 0 or isatty( fd_num ) ) {
                return;
              }

              FileDescriptor fd { fd_num };

              if ( ThunkPlaceholder::is_placeholder( move( fd ) ) ) {
                throw runtime_error( "Somebody tried to open a thunk: " + open_path );
              }
            }

            break;
          }
        },

        [&]( const TraceControlBlock & ) {}
      );

      if ( not waitres ) { break; }
    }

    if ( tp.exit_status().initialized() ) {
      cerr << "Process exited with " << tp.exit_status().get() << endl;
    }
  }
  catch ( const exception &  e ) {
    print_exception( argv[ 0 ], e );
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
