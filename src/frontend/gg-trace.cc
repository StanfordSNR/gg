/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <unistd.h>
#include <cstring>
#include <iostream>

#include "exception.hh"
#include "syscall.hh"
#include "traced_process.hh"

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
        [&]( TraceControlBlock & tcb )
        {
          tcb.syscall_invocation->fetch_arguments();
          cerr << tcb.to_string() << endl;
        },
        [&]( const TraceControlBlock & tcb )
        {
          assert( tcb.syscall_invocation.initialized() and
                  tcb.syscall_invocation->retval().initialized() );
          cerr << " = " << *tcb.syscall_invocation->retval() << endl;
        }
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
