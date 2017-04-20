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
    pid_ = CheckSystemCall( "waitpid", wait(&child_ret) );//waitpid( pid_, &child_ret, 0 ) );

    CheckSystemCall( "ptrace(SETOPTIONS)", ptrace( PTRACE_SETOPTIONS, pid_, 0,
                                                   PTRACE_O_TRACEEXEC | PTRACE_O_TRACESYSGOOD
                                                   | PTRACE_O_TRACEFORK  | PTRACE_O_TRACEVFORK
                                                   | PTRACE_O_TRACECLONE) );
  }
}

void TracedProcess::resume()
{
  process_state_ = RUNNING;
  CheckSystemCall( "ptrace(CONT)", ptrace( PTRACE_CONT, pid_, NULL, NULL ) );
}

TracedProcess::~TracedProcess()
{}

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

int ptrace_syscall( pid_t* pid )
{
  int status;

  while ( true ) {
    CheckSystemCall( "ptrace(SYSCALL)", ptrace( PTRACE_SYSCALL, *pid, 0, 0 ) );
    *pid = wait( &status );

    cerr << "New pid? " << *pid << endl;

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

  if ( ptrace_syscall( &pid_ ) != 0 ) {
    process_state_ = TERMINATED;
    return false;
  }

  process_state_ = STOPPED_FOR_SYSCALL;

  before_entry( SystemCall::get_syscall( ptrace( PTRACE_PEEKUSER, pid_, sizeof( long ) * ORIG_RAX ) ) );

  process_state_ = RUNNING_SYSCALL;
  if ( ptrace_syscall( &pid_ ) != 0 ) {
    process_state_ = TERMINATED;
    return false;
  }

  process_state_ = STOPPED_FOR_SYSCALL;
  after_exit( SystemCall::get_syscall( ptrace( PTRACE_PEEKUSER, pid_, sizeof( long ) * ORIG_RAX ) ),
              ptrace( PTRACE_PEEKUSER, pid_, sizeof( long ) * RAX ) );


  return true;
}

template<typename T>
T TracedProcess::get_syscall_arg( uint8_t argnum )
{
  if ( not ( process_state_ == RUNNING_SYSCALL || process_state_ == STOPPED_FOR_SYSCALL ) ) {
    throw runtime_error( "not running a syscall, can't get the args." );
  }

  return ( T ) ptrace( PTRACE_PEEKUSER, pid_, sizeof( long ) * SYSCALL_ARG_REGS[ argnum ], NULL );
}

template<>
string TracedProcess::get_syscall_arg( uint8_t argnum )
{

  cerr << "Pid: " << pid_ << endl;
  string result;

  char * str_addr = get_syscall_arg<char *>( argnum );
  size_t i = 0;

  do {
    int val = CheckSystemCall( "ptrace(PEEKTEXT)", ptrace( PTRACE_PEEKTEXT, pid_, str_addr, NULL ) );
    str_addr += sizeof( int );

    char * p = reinterpret_cast<char *>( &val );
    char c;

    for ( i = 0; i < sizeof( int ); i++ ) {
      c = *p++;
      if ( c == '\0' ) break;

      result += c;
    }
    // cerr << (c == '\0') << endl;
  } while( i == sizeof( int ) );

  return result;
}
