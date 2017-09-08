/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <libgen.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <cstring>
#include <unordered_map>
#include <sys/ptrace.h>

#include "tracer_thread.hh"
#include "exception.hh"
#include "placeholder.hh"
#include "file_descriptor.hh"
#include "child_process.hh"

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

    ChildProcess tp( argv[ 1 ],
                     [&argv]() {
                       CheckSystemCall( "ptrace(TRACEME)", ptrace( PTRACE_TRACEME ) );
                       raise( SIGSTOP );
                       return execvp( argv[ 1 ], &argv[ 1 ] ); } );

    TracerFlock tracers {
      [&]( TracedThreadInfo & tcb )
        {
          Optional<SystemCallInvocation> & invocation = tcb.syscall_invocation;

          switch ( invocation->syscall_no() ) {
          case SYS_open:
            {
              invocation->fetch_arguments();
              string open_path = invocation->arguments()->at( 0 ).value<string>();

              if ( open_path == "/__gg__detach_from_tracing__/" ) {
                cerr << "detaching!\n";
                tcb.detach = true;
                break;
              }

              cerr << invocation->to_string() << endl;

              /* if process wants to truncate the file, we don't
                 care whether it's a thunk placeholder or not */
              const int open_flags = invocation->arguments()->at( 1 ).value<int>();
              if ( open_flags & O_TRUNC ) {
                break;
              }

              /* otherwise: does the file
                 (1) exist,
                 (2) as a regular file,
                 (3) that we can open,
                 (4) and as a thunk placeholder? */

              struct stat stat_buf;
              const int stat_ret = stat( open_path.c_str(), &stat_buf );
              if ( stat_ret == 0 ) {
                /* (1) it exists -- is it a regular file? */
                if ( S_ISREG( stat_buf.st_mode ) ) {
                  /* (2) it's a regular file -- can we open it? */
                  const int fd_num = open( open_path.c_str(), O_RDONLY );
                  if ( fd_num >= 0 ) {
                    /* (3) successfully opened -- is it a thunk placeholder? */
                    if ( ThunkPlaceholder::is_placeholder( FileDescriptor { fd_num } ) ) {
                      throw runtime_error( "Attempt to open thunk placeholder: " + open_path );
                    }
                  }
                }
              }
            }
            break;
          }
        },

        [&]( const TracedThreadInfo & ) {}
    };

    tracers.insert( tp.pid() );

    tracers.loop_until_all_done();

    while ( not tp.terminated() ) {
      tp.wait();
    }

    if ( tp.exit_status() ) {
      tp.throw_exception();
    }
  }
  catch ( const exception &  e ) {
    print_exception( argv[ 0 ], e );
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
