/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <iostream>
#include <string>
#include <unordered_map>

#include "traced_process.hh"
#include "exception.hh"

using namespace std;

void usage( const char * argv0 )
{
  cerr << argv0 << " COMMAND [option]..." << endl;
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
        [&]( const TraceControlBlock & tcb )
        {
          #ifdef SYS_execveat
          if ( tcb.syscall_invocation->syscall_no() == SYS_execveat ) {
            throw runtime_error( "execveat() is not supported yet" );
          }
          #endif

          if ( tcb.syscall_invocation->syscall_no() == SYS_execve ) {
            cerr << "execve(\""
            << tcb.syscall_invocation->arguments()[ 0 ].value<string>()
            << "\")" << endl;
          }
        },
        [&]( const TraceControlBlock & ) {}
      );

      if ( not waitres ) { break; }
    }

    if ( tp.exit_status().initialized() ) {
      cerr << endl << "Process exited with " << tp.exit_status().get() << endl;
    }
  }
  catch ( const exception &  e ) {
    print_exception( argv[ 0 ], e );
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
