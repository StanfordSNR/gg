/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/reg.h>
#include <sys/user.h>
#include <signal.h>
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

    TracedProcess tp( &argv[ 1 ] );

    while ( true ) {
      int waitres = tp.wait_for_syscall(
        [&]( TraceControlBlock tcb, long, SystemCallEntry syscall )
        {
          cerr << "[" << tcb.pid << "] " << syscall.sys_name << "(...) called. " << endl;
        },
        [&]( TraceControlBlock tcb, long, SystemCallEntry syscall, long retval )
        {
          cerr << "[" << tcb.pid << "] " << syscall.sys_name << "(...) = " << retval << endl;
        }
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
