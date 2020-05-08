/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "sandbox.hh"

#include <iostream>
#include <functional>
#include <fcntl.h>

using namespace std;

SandboxedProcess::SandboxedProcess( const string & name,
                                    const string & working_directory,
                                    const unordered_map<string, Permissions> & allowed_files,
                                    function<int()> && child_procedure,
                                    function<void()> && preparation_procedure )
  : tracer_( name, move( child_procedure ),
             bind( &SandboxedProcess::syscall_entry, this, placeholders::_1 ),
             bind( &SandboxedProcess::syscall_exit,  this, placeholders::_1 ),
             move( preparation_procedure ) ),
    working_directory_( working_directory )
{
  for ( const auto & allowed_file : allowed_files ) {
    allow_file( allowed_file.first, allowed_file.second );
  }
}

inline void Check( const TracedThreadInfo & tcb, bool status )
{
  if ( not status ) {
    throw SandboxViolation( "Illegal syscall: ", tcb.syscall_invocation->to_string() );
  }
}

void SandboxedProcess::allow_file( const string & pathname, const Permissions permissions )
{
  struct stat statbuf;
  allowed_paths_[ pathname ] = permissions;

  if ( fstatat( working_directory_.num(), pathname.c_str(), &statbuf, 0 ) == 0 ) {
    allowed_inodes_[ make_pair( statbuf.st_dev, statbuf.st_ino ) ] = permissions;
  }
}

Optional<Permissions> SandboxedProcess::get_permissions( const string & pathname )
{
  if ( allowed_paths_.count( pathname ) ) {
    return { true, allowed_paths_[ pathname ] };
  }

  struct stat statbuf;
  if ( fstatat( working_directory_.num(), pathname.c_str(), &statbuf, 0 ) == 0
       and allowed_inodes_.count( make_pair( statbuf.st_dev, statbuf.st_ino ) ) ) {
    return { true, allowed_inodes_[ make_pair( statbuf.st_dev, statbuf.st_ino ) ] };
  }

  return {};
}

bool SandboxedProcess::file_syscall_entry( const SystemCallInvocation & syscall )
{
  assert( syscall.arguments().initialized() );

  for ( const Argument arg : *syscall.arguments() ) {
    if ( arg.info().flags & ARGUMENT_F_PATHNAME ) { /* it's a path argument */
      const string pathname = arg.value<string>();
      if ( not get_permissions( pathname ).initialized() ) {
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

  const Optional<Permissions> file_flags = get_permissions( pathname );

  if ( not file_flags.initialized() ) {
    if ( ( mode & O_CREAT ) and ( mode & O_EXCL ) ) {
      allow_candidates_.insert( pathname );
      return true;
    }

    return false;
  }

  if ( access_mode == O_RDWR and
       ( not file_flags->read or not file_flags->write ) ) { return false; }
  if ( access_mode == O_WRONLY and ( not file_flags->write ) ) { return false; }
  if ( access_mode == O_RDONLY and ( not file_flags->read ) ) { return false; }

  return true;
}

bool SandboxedProcess::execve_entry( const SystemCallInvocation & syscall )
{
  assert( syscall.arguments().initialized() );

  const string pathname = syscall.arguments()->at( 0 ).value<string>();

  const Optional<Permissions> file_flags = get_permissions( pathname );

  if ( not file_flags.initialized() ) {
    return false;
  }

  return file_flags->execute;
}

bool SandboxedProcess::open_exit( const SystemCallInvocation & syscall )
{
  assert( syscall.arguments().initialized() );

  const string pathname = syscall.arguments()->at( 0 ).value<string>();

  if ( allow_candidates_.count( pathname ) and *syscall.retval() != -1 ) {
    allow_candidates_.erase( pathname );
    allow_file( pathname, { true, true, true } );
  }

  return true;
}

bool SandboxedProcess::rename_entry( const SystemCallInvocation & syscall )
{
  assert( syscall.arguments().initialized() );

  const string src = syscall.arguments()->at( 0 ).value<string>();
  const string dst = syscall.arguments()->at( 1 ).value<string>();

  const auto src_perms = get_permissions( src );
  const auto dst_perms = get_permissions( dst );

  return ( src_perms.initialized() ) and ( src_perms->read ) and
         ( dst_perms.initialized() ) and ( dst_perms->write );
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
