/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <libgen.h>
#include <iostream>
#include <string>
#include <unordered_map>

#include "traced_process.hh"
#include "exception.hh"

using namespace std;

/* XXX move this to a configuration file */
unordered_map<string, string> model = {
  { "ls", "/bin/pwd" },
};

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

          #ifdef SYS_execveat
          if ( invocation->syscall_no() == SYS_execveat ) {
            throw runtime_error( "execveat() is not supported yet" );
          }
          #endif

          if ( invocation->syscall_no() == SYS_execve ) {
            string exe_path = invocation->arguments()[ 0 ].value<string>();

            /* XXX we should move to c++17 to use filesystem library for this */
            vector<char> path_cstr( exe_path.c_str(), exe_path.c_str() + exe_path.size() + 1 );
            string exe_filename { basename( path_cstr.data() ) };

            if ( model.count( exe_filename ) ) {
              invocation->set_argument( 0, model[ exe_filename ] );
            }
            else {
              throw runtime_error( "could not find a model for " + exe_filename );
            }
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
