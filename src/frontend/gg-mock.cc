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

#include "thunk/ggutils.hh"
#include "thunk/placeholder.hh"
#include "trace/tracer.hh"
#include "util/child_process.hh"
#include "util/exception.hh"
#include "util/file_descriptor.hh"
#include "util/system_runner.hh"

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

    Tracer tracer {
      argv[ 1 ],
      [&argv] { return execvp( argv[ 1 ], &argv[ 1 ] ); },
      [&]( TracedThreadInfo & tcb, TracerFlock & flock )
        {
          Optional<SystemCallInvocation> & invocation = tcb.syscall_invocation;

          switch ( invocation->syscall_no() ) {
          case SYS_open:
            {
              invocation->fetch_arguments();
              string open_path = invocation->arguments()->at( 0 ).value<string>();

              if ( open_path == gg::models::OPEN_TO_DETACH_PATH ) {
                tcb.detach = true;
                break;
              }

              // cerr << invocation->to_string() << endl;

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
                      /* (4) it's a thunk placeholder! let's force it */
                      tcb.pause = true;
                      ChildProcess reducer { "gg-force " + open_path,
                          [&](){ return ezexec( "gg-force", { "gg-force", open_path },
                                                {}, true, true ); } };
                      const pid_t reducer_pid = reducer.pid();
                      flock.add_child_process( move( reducer ) );
                      flock.resume_after_termination( reducer_pid, tcb.pid );
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

    tracer.loop_until_done();
  }
  catch ( const exception &  e ) {
    print_exception( argv[ 0 ], e );
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
