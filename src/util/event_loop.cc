/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "event_loop.hh"

#include <algorithm>
#include <numeric>

#include "exception.hh"

using namespace std;
using namespace PollerShortNames;

void assert_not_root( void )
{
    if ( ( geteuid() == 0 ) or ( getegid() == 0 ) ) {
        throw runtime_error( "BUG: privileges not dropped in sensitive region" );
    }
}

TemporarilyUnprivileged::TemporarilyUnprivileged()
  : orig_euid( geteuid() ),
    orig_egid( getegid() )
{
  CheckSystemCall( "setegid", setegid( getgid() ) );
  CheckSystemCall( "seteuid", seteuid( getuid() ) );

  assert_not_root();
}

TemporarilyUnprivileged::~TemporarilyUnprivileged()
{
  CheckSystemCall( "seteuid", seteuid( orig_euid ) );
  CheckSystemCall( "setegid", setegid( orig_egid ) );
}

string join( const vector< string > & command )
{
  return accumulate( command.begin() + 1, command.end(),
                     command.front(),
                     []( const string & a, const string & b )
                     { return a + " " + b; } );
}

EventLoop::EventLoop()
  : signals_( { SIGCHLD, SIGCONT, SIGHUP, SIGTERM, SIGQUIT, SIGINT } ),
   poller_(),
   child_processes_()
{
  signals_.set_as_mask(); /* block signals so we can later use signalfd to read them */
}

void EventLoop::add_simple_input_handler( FileDescriptor & fd,
                     const Poller::Action::CallbackType & callback )
{
  poller_.add_action( Poller::Action( fd, Direction::In, callback ) );
}

Result EventLoop::handle_signal( const signalfd_siginfo & sig )
{
  switch ( sig.ssi_signo ) {
  case SIGCONT:
    /* resume child processes too */
    for ( auto & x : child_processes_ ) {
      x.second.resume();
    }
    break;

  case SIGCHLD:
    if ( child_processes_.empty() ) {
      throw runtime_error( "received SIGCHLD without any managed children" );
    }

    /* find which children are waitable */
    /* we can't count on getting exactly one SIGCHLD per waitable event, so search */
    for ( auto & procpair : child_processes_ ) {
      ChildProcess & proc = procpair.second;
      if ( proc.terminated() or ( !proc.waitable() ) ) {
        continue; /* not the process we're looking for */
      }

      proc.wait( true ); /* get process's change of state.
                true => throws exception if no change available */

      if ( proc.terminated() ) {
        if ( proc.exit_status() != 0 and proc.exit_status() != procpair.first ) {
          proc.throw_exception();
        }

        /* quit if all children have quit */
        if ( all_of( child_processes_.begin(), child_processes_.end(),
               [] ( pair<int, ChildProcess> & x ) { return x.second.terminated(); } ) ) {
          return ResultType::Exit;
        }

        return proc.exit_status() == procpair.first ? ResultType::Continue : ResultType::Exit;
      } else if ( !proc.running() ) {
        /* suspend parent too */
        CheckSystemCall( "raise", raise( SIGSTOP ) );
      }
    }

    break;

  case SIGHUP:
  case SIGTERM:
  case SIGQUIT:
  case SIGINT:
    return ResultType::Exit;
  default:
    throw runtime_error( "EventLoop: unknown signal" );
  }

  return ResultType::Continue;
}

int EventLoop::internal_loop( const std::function<int(void)> & wait_time )
{
  TemporarilyUnprivileged tu;

  /* verify that signal mask is intact */
  SignalMask current_mask = SignalMask::current_mask();

  if ( !( signals_ == current_mask ) ) {
    throw runtime_error( "EventLoop: signal mask has been altered" );
  }

  SignalFD signal_fd( signals_ );

  /* we get signal -> main screen turn on */
  add_simple_input_handler( signal_fd.fd(),
                            [&] () { return handle_signal( signal_fd.read_signal() ); } );

  while ( true ) {
    const auto poll_result = poller_.poll( wait_time() );
    if ( poll_result.result == Poller::Result::Type::Exit ) {
      return poll_result.exit_status;
    }
  }
}
