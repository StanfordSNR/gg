/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "traced_process.hh"

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/reg.h>
#include <sys/user.h>
#include <sys/stat.h>
#include <signal.h>
#include <cstring>
#include <exception>

#include "exception.hh"

using namespace std;

int SYSCALL_ARG_REGS[] = { ORIG_RAX, RDI, RSI, RDX, R10, R8, R9 };

int do_fork()
{
  /* Verify that process is single-threaded before forking */
  {
    struct stat my_stat;
    CheckSystemCall( "stat", stat( "/proc/self/task", &my_stat ) );

    if ( my_stat.st_nlink != 3 ) {
      throw runtime_error( "ChildProcess constructed in multi-threaded program" );
    }
  }

  return CheckSystemCall( "fork", fork() );
}

TracedProcess::TracedProcess( char * args[], const int termination_signal )
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
    CheckSystemCall( "execvp", execvp( args[ 0 ], &args[ 0 ] ) );
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
                                                     PTRACE_O_TRACECLONE ) );
      processes_.at( cpid ).initialized = true;
    }

    if ( WIFSTOPPED( status ) && WSTOPSIG( status ) & 0x80 ) {
      return true;
    }
    else if ( WIFEXITED( status ) ) {
      return false;
    }

    CheckSystemCall( "ptrace(SYSCALL)", ptrace( PTRACE_SYSCALL, cpid, NULL, NULL ) );
  }
}

bool TracedProcess::wait_for_syscall( function<void( TraceControlBlock, long, SystemCallEntry )> before_entry,
                                      function<void( TraceControlBlock, long, SystemCallEntry, long )> after_exit )
{
  pid_t cpid;

  if ( not ptrace_syscall( cpid ) ) {
    return false;
  }

  TraceControlBlock & tcb = processes_.at( cpid );

  if ( not tcb.in_syscall ) {
    tcb.in_syscall = true;

    long syscall_no = ptrace( PTRACE_PEEKUSER, cpid, sizeof( long ) * ORIG_RAX );
    before_entry( tcb, syscall_no, SystemCall::get_syscall( syscall_no ) );
  }
  else {
    long syscall_no = ptrace( PTRACE_PEEKUSER, cpid, sizeof( long ) * ORIG_RAX );
    long syscall_ret = ptrace( PTRACE_PEEKUSER, cpid, sizeof( long ) * RAX );

    after_exit( tcb, syscall_no, SystemCall::get_syscall( syscall_no ), syscall_ret );

    tcb.in_syscall = false;
  }

  CheckSystemCall( "ptrace(SYSCALL)", ptrace( PTRACE_SYSCALL, cpid, 0, 0 ) );
  return true;
}

template<typename T>
T TracedProcess::get_syscall_arg( TraceControlBlock tcb, uint8_t argnum )
{
  assert( tcb.in_syscall );

  return ( T ) ptrace( PTRACE_PEEKUSER, tcb.pid, sizeof( long ) * SYSCALL_ARG_REGS[ argnum ], NULL );
}

template<>
string TracedProcess::get_syscall_arg( TraceControlBlock tcb, uint8_t argnum )
{
  string result;

  char * str_addr = get_syscall_arg<char *>( tcb, argnum );
  size_t i = 0;

  do {
    int val = CheckSystemCall( "ptrace(PEEKTEXT)", ptrace( PTRACE_PEEKTEXT, tcb.pid, str_addr, NULL ) );
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

template int TracedProcess::get_syscall_arg( TraceControlBlock, uint8_t );
