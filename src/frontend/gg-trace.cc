/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <unistd.h>
#include <cstring>
#include <iostream>
#include <sys/ptrace.h>

#include "trace/syscall.hh"
#include "trace/tracer.hh"
#include "util/child_process.hh"
#include "util/exception.hh"

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

    ChildProcess tp( argv[ 1 ],
                     [&argv]() {
                       CheckSystemCall( "ptrace(TRACEME)", ptrace( PTRACE_TRACEME ) );
                       raise( SIGSTOP );
                       return execvp( argv[ 1 ], &argv[ 1 ] ); } );

    cerr << "Direct child: " << tp.pid() << endl;

    TracerFlock tracers { []( TracedThreadInfo &, TracerFlock & )
        {
          //          tcb.syscall_invocation->fetch_arguments();
          //          cerr << tcb.syscall_invocation->to_string();
        },
        []( const TracedThreadInfo &  )
          {
            /*
            assert( tcb.syscall_invocation.initialized() and
                    tcb.syscall_invocation->retval().initialized() );
            cerr << " = " << *tcb.syscall_invocation->retval() << endl;
            */
          } };

    tracers.insert( tp.pid() );

    cerr << "Looping until all threads finish...\n";
    tracers.loop_until_all_done();

    cerr << "Waiting for direct child to finish... ";

    while ( !tp.terminated() ) {
      tp.wait();
    }

    cerr << "done.\n";

    if ( tp.exit_status() != 0 ) {
      tp.throw_exception();
    }
  }
  catch ( const exception &  e ) {
    print_exception( argv[ 0 ], e );
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
