/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "traced_process.hh"

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/reg.h>
#include <sys/user.h>
#include <sys/stat.h>
#include <signal.h>
#include <errno.h>
#include <cstring>
#include <sstream>
#include <exception>

#include "exception.hh"

using namespace std;

int SYSCALL_ARG_REGS[] = { /* ORIG_RAX, */ RDI, RSI, RDX, R10, R8, R9 };

extern int do_fork();

string TraceControlBlock::to_string() const
{
  ostringstream out;

  out << "[" << pid << "]";

  if ( syscall_invocation.initialized() ) {
    out << " " << syscall_invocation.get().to_string();
  }

  return out.str();
}

TracedProcess::TracedProcess( function<int()> && child_procedure,
                              const int termination_signal )
  : pid_( do_fork() ),
    exit_status_(),
    graceful_termination_signal_( termination_signal ),
    died_on_signal_( false ),
    moved_away_( false ),
    processes_()
{
  if ( pid_ == 0 ) { /* child */
    CheckSystemCall( "ptrace(TRACEME)", ptrace( PTRACE_TRACEME ) );
    CheckSystemCall( "kill", kill( getpid(), SIGSTOP ) );

    _exit( child_procedure() );
  }
  else {}
}

void TracedProcess::resume()
{
  CheckSystemCall( "ptrace(CONT)", ptrace( PTRACE_CONT, pid_, NULL, NULL ) );
}

TracedProcess::~TracedProcess()
{}

TracedProcess::TracedProcess( TracedProcess && other )
  : pid_( other.pid_ ),
    exit_status_( other.exit_status_ ),
    graceful_termination_signal_( other.graceful_termination_signal_ ),
    died_on_signal_( other.died_on_signal_ ),
    moved_away_( other.moved_away_ ),
    processes_( move( other.processes_ ) )
{
    assert( !other.moved_away_ );

    other.moved_away_ = true;
}

bool TracedProcess::ptrace_syscall( pid_t & cpid )
{
  int status;

  while ( true ) {
    cpid = CheckSystemCall( "waitpid", waitpid( -1, &status, __WALL ) );

    if ( processes_.count( cpid ) == 0 ) {
      processes_.insert( { cpid, { cpid } } );
    }

    if ( not processes_.at( cpid ).initialized ) {
      CheckSystemCall( "ptrace(SETOPTIONS)", ptrace( PTRACE_SETOPTIONS, cpid, 0,
                                                     PTRACE_O_TRACESYSGOOD |
                                                     PTRACE_O_TRACEEXEC |
                                                     PTRACE_O_TRACEEXIT |
                                                     PTRACE_O_TRACEFORK |
                                                     PTRACE_O_TRACEVFORK |
                                                     PTRACE_O_TRACECLONE |
                                                     PTRACE_O_EXITKILL ) );
      processes_.at( cpid ).initialized = true;
    }

    if ( WIFSTOPPED( status ) && WSTOPSIG( status ) & 0x80 ) {
      return true;
    }
    else if ( WIFEXITED( status ) ) {
      if ( cpid == pid_ ) { /* the parent process exited */
        exit_status_.reset( WEXITSTATUS( status ) );
      }

      processes_.erase( cpid );

      if ( cpid == pid_ ) {
        return false;
      }
      else {
        continue;
      }
    }

    CheckSystemCall( "ptrace(SYSCALL)", ptrace( PTRACE_SYSCALL, cpid, NULL, NULL ) );
  }
}

bool TracedProcess::wait_for_syscall( function<void( const TraceControlBlock & )> before_entry,
                                      function<void( const TraceControlBlock &, long )> after_exit )
{
  pid_t cpid;

  if ( not ptrace_syscall( cpid ) ) {
    return false;
  }

  TraceControlBlock & tcb = processes_.at( cpid );

  if ( not tcb.syscall_invocation.initialized() ) {
    long syscall_no = ptrace( PTRACE_PEEKUSER, cpid, sizeof( long ) * ORIG_RAX );
    tcb.syscall_invocation.reset( tcb.pid, syscall_no );
    before_entry( tcb );
  }
  else {
    long syscall_ret = ptrace( PTRACE_PEEKUSER, cpid, sizeof( long ) * RAX );
    after_exit( tcb, syscall_ret );

    tcb.syscall_invocation.clear();
  }

  CheckSystemCall( "ptrace(SYSCALL)", ptrace( PTRACE_SYSCALL, cpid, 0, 0 ) );
  return true;
}

template<typename T>
T TracedProcess::get_syscall_arg( const pid_t pid, uint8_t argnum )
{
  return ( T ) ptrace( PTRACE_PEEKUSER, pid,
                       sizeof( long ) * SYSCALL_ARG_REGS[ argnum ], NULL );
}

template<>
string TracedProcess::get_syscall_arg( const pid_t pid, uint8_t argnum )
{
  string result;

  char * str_addr = get_syscall_arg<char *>( pid, argnum );
  size_t i = 0;

  do {
    errno = 0;

    int val = ptrace( PTRACE_PEEKTEXT, pid, str_addr, NULL );

    if ( errno and val < 0 ) {
      throw unix_error( "ptrace(PEEKTEXT)" );
    }

    str_addr += sizeof( int );

    char * p = reinterpret_cast<char *>( &val );
    char c;

    for ( i = 0; i < sizeof( int ); i++ ) {
      c = *p++;
      if ( c == '\0' ) break;

      result += c;
    }
  } while( i == sizeof( int ) );

  return result;
}

user_regs_struct TracedProcess::get_regs( const pid_t pid )
{
  user_regs_struct result;
  CheckSystemCall( "ptrace(GETREGS)", ptrace( PTRACE_GETREGS, pid, NULL, &result ) );
  return result;
}

template int TracedProcess::get_syscall_arg( const pid_t, uint8_t );
template long TracedProcess::get_syscall_arg( const pid_t, uint8_t );
