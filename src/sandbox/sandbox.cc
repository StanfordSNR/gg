/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "sandbox.hh"

#include <iostream>
#include <functional>
#include <fcntl.h>

using namespace std;

SandboxedProcess::SandboxedProcess( const std::string & name,
                                    const unordered_map<std::string, Permissions> & allowed_files,
                                    function<int()> && child_procedure,
                                    function<void()> && preparation_procedure )
  : tracer_( name, move( child_procedure ),
             std::bind( &SandboxedProcess::syscall_entry, this, std::placeholders::_1 ),
             std::bind( &SandboxedProcess::syscall_exit,  this, std::placeholders::_1 ),
             move( preparation_procedure ) ),
    allowed_files_( allowed_files )
{}

inline void Check( const TracedThreadInfo & tcb, bool status )
{
  if ( not status ) {
    throw SandboxViolation( "Illegal syscall: ", tcb.syscall_invocation->to_string() );
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

bool SandboxedProcess::execve_entry( const SystemCallInvocation & syscall )
{
  assert( syscall.arguments().initialized() );

  const string pathname = syscall.arguments()->at( 0 ).value<string>();

  if ( not allowed_files_.count( pathname ) ) {
    return false;
  }

  Permissions file_flags = allowed_files_.at( pathname );
  return file_flags.execute;
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

bool SandboxedProcess::rename_entry( const SystemCallInvocation & syscall )
{
  assert( syscall.arguments().initialized() );

  const string src = syscall.arguments()->at( 0 ).value<string>();
  const string dst = syscall.arguments()->at( 1 ).value<string>();

  return ( allowed_files_.count( src ) > 0 ) and
         ( allowed_files_[ src ].read ) and
         ( allowed_files_.count( dst ) > 0 ) and
         ( allowed_files_[ dst ].write );
}

void SandboxedProcess::syscall_entry( TracedThreadInfo & tcb )
{
  SystemCallInvocation & syscall = tcb.syscall_invocation.get();

  switch ( syscall.syscall_no() ) {
  case SYS_chroot:
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
  case SYS_getpid:
  case SYS_gettimeofday:
  case SYS_time:
  case SYS_clock_gettime:
  case SYS_getcpu:
  case SYS_mkdir:
  case SYS_socket:
    throw SandboxViolation( "Forbidden syscall", tcb.syscall_invocation->to_string() );
  }

  if ( syscall.signature().initialized() ) {
    if ( not ( syscall.signature()->flags() & TRACE_FILE ) ) {
      return;
      /* this system call is not file-related. allow it! */
    }

    /* only fetch the arguments when it's necessary */
    syscall.fetch_arguments();

    switch ( syscall.syscall_no() ) {
    case SYS_open:   Check( tcb, open_entry( syscall ) ); break;
    case SYS_rename: Check( tcb, rename_entry( syscall ) ); break;
    case SYS_execve: Check( tcb, execve_entry( syscall ) ); break;

    /* general check for file-related syscalls */
    default: Check( tcb, file_syscall_entry( syscall ) );
    }
  }
  else {
    throw SandboxViolation( "Unknown syscall", tcb.syscall_invocation->to_string() );
  }
}

void SandboxedProcess::syscall_exit( const TracedThreadInfo & tcb )
{
  const SystemCallInvocation & syscall = tcb.syscall_invocation.get();

  switch ( syscall.syscall_no() ) {
  case SYS_open:
    Check( tcb, open_exit( syscall ) );
    break;
  }
}

void SandboxedProcess::execute()
{
  tracer_.loop_until_done();
}
