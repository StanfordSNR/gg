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
    process_state_( NOT_STARTED ),
    exit_status_(),
    graceful_termination_signal_( termination_signal ),
    died_on_signal_( false ),
    moved_away_( false )
{
  if ( pid_ == 0 ) { /* child */
    CheckSystemCall( "ptrace(TRACEME)", ptrace( PTRACE_TRACEME ) );
    kill( getpid(), SIGSTOP );
    CheckSystemCall( "execvp", execvp( args[ 0 ], &args[ 0 ] ) );
  }
  else {
    int child_ret;
    CheckSystemCall( "waitpid", waitpid( pid_, &child_ret, 0 ) );

    CheckSystemCall( "ptrace(SETOPTIONS)", ptrace( PTRACE_SETOPTIONS, pid_, 0,
                                                   PTRACE_O_TRACEEXEC | PTRACE_O_TRACESYSGOOD ) );
  }
}

void TracedProcess::resume()
{
  process_state_ = RUNNING;
  CheckSystemCall( "ptrace(CONT)", ptrace( PTRACE_CONT, pid_, NULL, NULL ) );
}

/* send a signal to the child process */
void TracedProcess::signal( const int sig )
{
  assert( !moved_away_ );

  if ( process_state_ != TERMINATED ) {
    CheckSystemCall( "kill", kill( pid_, sig ) );
  }
}

template <typename T> void zero( T & x ) { memset( &x, 0, sizeof( x ) ); }

/* wait for process to change state */
void TracedProcess::wait( const bool nonblocking )
{
  assert( !moved_away_ );
  assert( !terminated_ );

  siginfo_t infop;
  zero( infop );
  CheckSystemCall( "waitid", waitid( P_PID, pid_, &infop,
                                     WEXITED | WSTOPPED | WCONTINUED | (nonblocking ? WNOHANG : 0) ) );

  if ( nonblocking and (infop.si_pid == 0) ) {
    throw runtime_error( "nonblocking wait: process was not waitable" );
  }

  if ( infop.si_pid != pid_ ) {
    throw runtime_error( "waitid: unexpected value in siginfo_t si_pid field" );
  }

  if ( infop.si_signo != SIGCHLD ) {
    throw runtime_error( "waitid: unexpected value in siginfo_t si_signo field (not SIGCHLD)" );
  }

  /* how did the process change state? */
  switch ( infop.si_code ) {
  case CLD_EXITED:
    process_state_ = TERMINATED;
    exit_status_ = infop.si_status;
    break;
  case CLD_KILLED:
  case CLD_DUMPED:
      process_state_ = TERMINATED;
      exit_status_ = infop.si_status;
      died_on_signal_ = true;
      break;
  case CLD_STOPPED:
      process_state_ = STOPPED;
      break;
  case CLD_CONTINUED:
      process_state_ = RUNNING;
      break;
  default:
      throw runtime_error( "waitid: unexpected siginfo_t si_code" );
  }
}

TracedProcess::~TracedProcess()
{
  if ( moved_away_ ) { return; }

  try {
    while ( process_state_ != TERMINATED ) {
      resume();
      signal( graceful_termination_signal_ );
      wait();
    }
  } catch ( const exception & e ) {
    print_exception( "~TracedProcess", e );
  }
}

TracedProcess::TracedProcess( TracedProcess && other )
  : pid_( other.pid_ ),
    process_state_( other.process_state_ ),
    exit_status_( other.exit_status_ ),
    graceful_termination_signal_( other.graceful_termination_signal_ ),
    died_on_signal_( other.died_on_signal_ ),
    moved_away_( other.moved_away_ )
{
    assert( !other.moved_away_ );

    other.moved_away_ = true;
}

int ptrace_syscall( pid_t pid )
{
  int status;

  while ( true ) {
    CheckSystemCall( "ptrace(SYSCALL)", ptrace( PTRACE_SYSCALL, pid, 0, 0 ) );
    waitpid( pid, &status, 0 );

    if ( WIFSTOPPED( status ) && WSTOPSIG( status ) & 0x80 ) {
      return 0;
    }
    else if ( WIFEXITED( status ) ) {
      return 1;
    }
  }
}

bool TracedProcess::wait_for_syscall( function<void( SystemCallEntry )> before_entry,
                                      function<void( SystemCallEntry, long int )> after_exit )
{
  process_state_ = RUNNING;
  if ( ptrace_syscall( pid_ ) != 0 ) {
    process_state_ = TERMINATED;
    return false;
  }

  process_state_ = STOPPED;
  before_entry( SystemCall::get_syscall( ptrace( PTRACE_PEEKUSER, pid_, 8 * ORIG_RAX ) ) );

  process_state_ = RUNNING;
  if ( ptrace_syscall( pid_ ) != 0 ) {
    process_state_ = TERMINATED;
    return false;
  }

  process_state_ = STOPPED;
  after_exit( SystemCall::get_syscall( ptrace( PTRACE_PEEKUSER, pid_, 8 * ORIG_RAX ) ),
              ptrace( PTRACE_PEEKUSER, pid_, 8 * RAX ) );

  return true;
}
