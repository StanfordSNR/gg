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
    moved_away_( false ),
    in_syscall_pids_()
{
  if ( pid_ == 0 ) { /* child */
    CheckSystemCall( "ptrace(TRACEME)", ptrace( PTRACE_TRACEME ) );
    kill( getpid(), SIGSTOP );
    CheckSystemCall( "execvp", execvp( args[ 0 ], &args[ 0 ] ) );
  }
  else {
    waitpid( pid_, NULL, __WALL );

    CheckSystemCall( "ptrace(SETOPTIONS)", ptrace( PTRACE_SETOPTIONS, pid_, 0,
                                                   PTRACE_O_TRACEEXEC |
                                                   PTRACE_O_TRACESYSGOOD |
                                                   PTRACE_O_TRACEFORK |
                                                   PTRACE_O_TRACEVFORK |
                                                   PTRACE_O_TRACECLONE ) );

    ptrace( PTRACE_SYSCALL, pid_, NULL, NULL );
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
    moved_away_( other.moved_away_ ),
    in_syscall_pids_( move( other.in_syscall_pids_ ) )
{
    assert( !other.moved_away_ );

      cerr << "Clone syscall!" << endl;
    other.moved_away_ = true;
}

bool TracedProcess::ptrace_syscall( pid_t * cpid )
{
  int status;

  while ( true ) {
    *cpid = waitpid( -1, &status, __WALL );

    if ( WIFSTOPPED( status ) && WSTOPSIG( status ) & 0x80 ) {
      return true;
    }
    else if ( WIFEXITED( status ) ) {
      return false;
    }

    ptrace( PTRACE_SYSCALL, *cpid, NULL, NULL );
  }
}

bool TracedProcess::wait_for_syscall( function<void( long, SystemCallEntry )> before_entry,
                                      function<void( long, SystemCallEntry, long )> after_exit )
{
  pid_t cpid;

  if ( not ptrace_syscall( &cpid ) ) {
    return false;
  }

  if ( in_syscall_pids_.count( cpid ) == 0 ) {
    in_syscall_pids_.insert( cpid );

    long syscall_no = ptrace( PTRACE_PEEKUSER, cpid, sizeof( long ) * ORIG_RAX );
    before_entry( syscall_no, SystemCall::get_syscall( syscall_no ) );
  }
  else {
    long syscall_no = ptrace( PTRACE_PEEKUSER, pid_, sizeof( long ) * ORIG_RAX );
    long syscall_ret = ptrace( PTRACE_PEEKUSER, pid_, sizeof( long ) * RAX );

    after_exit( syscall_no, SystemCall::get_syscall( syscall_no ), syscall_ret );

    if ( syscall_no == SYS_clone && syscall_ret > 0 ) {
      ptrace( PTRACE_SETOPTIONS, syscall_ret + 1, 0,
              PTRACE_O_TRACEEXEC |
              PTRACE_O_TRACESYSGOOD |
              PTRACE_O_TRACEFORK |
              PTRACE_O_TRACEVFORK |
              PTRACE_O_TRACECLONE );

      ptrace( PTRACE_SYSCALL, syscall_ret + 1, NULL, NULL );
    }

    in_syscall_pids_.erase( cpid );
  }

  CheckSystemCall( "ptrace(SYSCALL)", ptrace( PTRACE_SYSCALL, cpid, 0, 0 ) );
  return true;
}

template<typename T>
T TracedProcess::get_syscall_arg( uint8_t argnum )
{
  return ( T ) ptrace( PTRACE_PEEKUSER, pid_, sizeof( long ) * SYSCALL_ARG_REGS[ argnum ], NULL );
}

template<>
string TracedProcess::get_syscall_arg( uint8_t argnum )
{
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
  } while( i == sizeof( int ) );

  return result;
}

template int TracedProcess::get_syscall_arg( uint8_t argnum );
