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
  assert( syscall.arguments().initialized() );

  for ( const Argument arg : *syscall.arguments() ) {
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
  assert( syscall.arguments().initialized() );

  const string pathname = syscall.arguments()->at( 0 ).value<string>();
  const int mode = syscall.arguments()->at( 1 ).value<int>();
  const int access_mode = mode & O_ACCMODE;

  if ( not allowed_files_.count( pathname ) ) {
    if ( ( mode & O_CREAT ) and ( mode & O_EXCL ) ) {
      allow_candidates_.insert( pathname );
      return true;
    }

    return false;
  }

  Permissions file_flags = allowed_files_.at( pathname );

  if ( access_mode == O_RDWR and
       ( not file_flags.read or not file_flags.write ) ) { return false; }
  if ( access_mode == O_WRONLY and ( not file_flags.write ) ) { return false; }
  if ( access_mode == O_RDONLY and ( not file_flags.read ) ) { return false; }

  return true;
}

bool SandboxedProcess::open_exit( const SystemCallInvocation & syscall )
{
  assert( syscall.arguments().initialized() );

  const string pathname = syscall.arguments()->at( 0 ).value<string>();

  if ( allow_candidates_.count( pathname ) and *syscall.retval() != -1 ) {
    allow_candidates_.erase( pathname );
    allowed_files_[ pathname ] = { true, true, true };
  }

  return true;
}

void SandboxedProcess::execute()
{
  auto syscall_entry =
    [&]( TraceControlBlock & tcb )
    {
      if ( log_level_ >= LOG_LEVEL_DEBUG ) {
        cerr << tcb.to_string() << endl;
      }

      SystemCallInvocation & syscall = tcb.syscall_invocation.get();

      switch ( syscall.syscall_no() ) {
      case SYS_chroot:
      case SYS_rename:
      case SYS_stat:
      case SYS_lstat:
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
        if ( not ( syscall.signature()->flags() & TRACE_FILE ) ) {
          return;
          /* this system call is not file-related. allow it! */
        }

        /* only fetch the arguments when it's necessary */
        syscall.fetch_arguments();

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
    [&]( const TraceControlBlock & tcb )
    {
      const SystemCallInvocation & syscall = tcb.syscall_invocation.get();

      switch ( syscall.syscall_no() ) {
      case SYS_open:
        Check( tcb, open_exit( syscall ) );
        break;
      }
    };

  while ( true ) {
    int waitres = process_.wait_for_syscall( syscall_entry, syscall_exit );
    if ( not waitres ) { break; }
  }
}
