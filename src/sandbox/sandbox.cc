/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "sandbox.hh"

#include <fcntl.h>
#include <iostream>

using namespace std;

SandboxedProcess::SandboxedProcess( function<int()> && child_procedure,
                                    const unordered_map<std::string, Permissions> & allowed_files )
  : process_( move( child_procedure ) ), allowed_files_( allowed_files )
{}

inline void Check( const TraceControlBlock & tcb, bool status )
{
  if ( not status ) {
    throw SandboxViolation( "Illegal syscall: ", tcb.to_string() );
  }
}

bool SandboxedProcess::file_syscall_entry( const SystemCallInvocation & syscall )
{
  for ( const Argument & arg : syscall.arguments() ) {
    if ( arg.info().flags & ARGUMENT_F_PATHNAME ) { /* it's a path argument */
      const string pathname = arg.value<string>();
      if ( allowed_files_.count( pathname ) == 0 ) {
        return false;
      }
    }
  }

  return true;
}

bool SandboxedProcess::open_entry( const SystemCallInvocation & syscall )
{
  const string pathname = syscall.arguments().at( 0 ).value<string>();
  const int access_mode = syscall.arguments().at( 1 ).value<int>() & O_ACCMODE;

  if ( not allowed_files_.count( pathname ) ) {
    return false;
  }

  Permissions file_flags = allowed_files_.at( pathname );

  if ( access_mode == O_RDWR and
       ( not file_flags.read or not file_flags.write ) ) { return false; }
  if ( access_mode == O_WRONLY and ( not file_flags.write ) ) { return false; }
  if ( access_mode == O_RDONLY and ( not file_flags.read ) ) { return false; }

  return true;
}

void SandboxedProcess::execute()
{
  auto syscall_entry =
    [&]( const TraceControlBlock & tcb )
    {
      if ( log_level_ >= LOG_LEVEL_DEBUG ) {
        cerr << tcb.to_string() << endl;
      }

      const SystemCallInvocation & syscall = tcb.syscall_invocation.get();

      switch ( syscall.syscall_no() ) {
      case SYS_chroot:
      case SYS_chdir:
      case SYS_fchdir:
      case SYS_openat:
      case SYS_mkdirat:
      case SYS_mknodat:
      case SYS_fchownat:
      case SYS_futimesat:
      case SYS_newfstatat:
      case SYS_unlinkat:
      case SYS_renameat:
      case SYS_linkat:
      case SYS_symlinkat:
      case SYS_readlinkat:
      case SYS_fchmodat:
      case SYS_faccessat:
      case SYS_utimensat:
      case SYS_name_to_handle_at:
      case SYS_open_by_handle_at:
#ifdef SYS_execveat
      case SYS_execveat:
#endif
      case SYS_fanotify_mark:
      case SYS_renameat2:
        throw SandboxViolation( "Forbidden syscall", tcb.to_string() );
      }

      if ( syscall.signature().initialized() ) {
        SystemCallSignature & signature = syscall.signature().get();

        if ( not ( signature.flags() & TRACE_FILE ) ) {
          return;
          /* this system call is not file-related. allow it! */
        }

        switch ( syscall.syscall_no() ) {
        case SYS_open:
          Check( tcb, open_entry( syscall ) );
          break;

        default:
          /* general check for file-related syscalls */
          Check( tcb, file_syscall_entry( syscall ) );
        }
      }
      else {
        throw SandboxViolation( "Unknown syscall", tcb.to_string() );
      }
    };

  auto syscall_exit =
    [&]( const TraceControlBlock &, long )
    {};

  while ( true ) {
    int waitres = process_.wait_for_syscall( syscall_entry, syscall_exit );
    if ( not waitres ) { break; }
  }
}
