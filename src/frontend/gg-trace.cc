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

void print_invocation( const TraceControlBlock & tcb )
{
  cerr << "[" << tcb.pid << "] ";

  if ( tcb.syscall_invocation.get().signature().initialized() ) {
    cerr << tcb.syscall_invocation.get().signature().get().name() << "(";

    size_t i = 0;
    for ( auto & arg : tcb.syscall_invocation.get().arguments() ) {
      i++;

      if ( arg.info().type == typeid( char * ) ) {
        cerr << '"' << arg.value<string>() << '"';
      }
      else {
        cerr << arg.value<long>();
      }

      if ( i != tcb.syscall_invocation.get().arguments().size() ) {
        cerr << ", ";
      }
    }

    cerr << ")";
  }
  else {
    cerr << "scno_" << tcb.syscall_invocation.get().syscall_no() << "()";
  }
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
          print_invocation( tcb );
        },
        [&]( const TraceControlBlock &, long retval )
        {
          cerr << " = " << retval << endl;
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
