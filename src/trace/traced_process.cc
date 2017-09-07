/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "traced_process.hh"

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <sys/stat.h>
#include <linux/limits.h>
#include <signal.h>
#include <errno.h>
#include <cstring>
#include <sstream>
#include <exception>

#include "exception.hh"

using namespace std;

extern int do_fork();

string TraceControlBlock::to_string() const
{
  ostringstream out;

  out << "[" << pid << "]";

  if ( syscall_invocation.initialized() ) {
    out << " " << syscall_invocation->to_string();
  }

  return out.str();
}

TracedProcess::TracedProcess( function<int()> && child_procedure,
                              function<void()> && preparation_procedure,
                              const int termination_signal )
  : pid_( do_fork() ),
    exit_status_(),
    graceful_termination_signal_( termination_signal ),
    died_on_signal_( false ),
    moved_away_( false ),
    processes_()
{
  if ( pid_ == 0 ) { /* child */
    preparation_procedure();

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

    int signum = 0;
    unsigned int event = (unsigned int) status >> 16;

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

    if ( WIFSTOPPED( status ) and ( WSTOPSIG( status ) & 0x80 ) ) {
      /* the process is stopped AND it's a syscall */
      return true;
    }
    else if ( WIFEXITED( status ) or WIFSIGNALED( status ) ) {
      /* the process is terminated */
      if ( WIFEXITED( status ) and cpid == pid_ ) { /* the parent process exited */
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
    else if ( WIFSTOPPED( status ) ) {
      /* the process is stopped, but it's not a syscall--it's because of a
         signal */
      signum = WSTOPSIG( status );

      switch ( event ) {
      case 0:
        break;

      case PTRACE_EVENT_EXIT:
      default:
        signum = 0;
        break;
      }
    }
    else {
      throw runtime_error( "process did not stop" );
    }

    CheckSystemCall( "ptrace(SYSCALL)", ptrace( PTRACE_SYSCALL, cpid, NULL, signum ) );
  }
}

bool TracedProcess::wait_for_syscall( function<void( TraceControlBlock & )> before_entry,
                                      function<void( const TraceControlBlock & )> after_exit )
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
    tcb.syscall_invocation->set_retval( syscall_ret );
    after_exit( tcb );

    tcb.syscall_invocation.clear();
  }

  if ( tcb.detached() ) {
    CheckSystemCall( "ptrace(DETACH)", ptrace( PTRACE_DETACH, cpid, 0, 0 ) );
    processes_.erase( cpid );
  }
  else {
    CheckSystemCall( "ptrace(SYSCALL)", ptrace( PTRACE_SYSCALL, cpid, 0, 0 ) );
  }

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

template<typename T>
void TracedProcess::set_syscall_arg( const pid_t pid, uint8_t argnum, const T & value )
{
  ptrace( PTRACE_POKEUSER, pid,
          sizeof( long ) * SYSCALL_ARG_REGS[ argnum ], &value );
}

template<>
void TracedProcess::set_syscall_arg( const pid_t pid, uint8_t argnum, const string & value )
{
  if ( value.length() >= PATH_MAX ) {
    throw runtime_error( "maximum string length for set_syscall_arg is PATH_MAX" );
  }

  char * stack_addr = ( char * )ptrace( PTRACE_PEEKUSER, pid, sizeof( long ) * RSP, 0 );
  stack_addr -= 128 + PATH_MAX * ( argnum + 1 );

  char * const str_addr = stack_addr;

  size_t i = 0;
  long val = 0;
  char * val_ptr = reinterpret_cast<char *>( &val );

  for ( const char * c = value.c_str(); ; c++ ) {
    val_ptr[ i++ ] = *c;

    if ( i == sizeof( long ) or *c == '\0' ) {
      ptrace( PTRACE_POKETEXT, pid, stack_addr, val );

      stack_addr += i;
      i = 0;
      val = 0;
    }

    if ( *c == '\0' ) {
      break;
    }
  }

  ptrace( PTRACE_POKEUSER, pid, sizeof( long ) * SYSCALL_ARG_REGS[ argnum ], str_addr );
}

template int TracedProcess::get_syscall_arg( const pid_t, uint8_t );
template long TracedProcess::get_syscall_arg( const pid_t, uint8_t );
