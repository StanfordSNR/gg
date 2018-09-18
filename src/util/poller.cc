/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <algorithm>
#include <numeric>
#include <chrono>

#include "poller.hh"
#include "exception.hh"

using namespace std;
using namespace PollerShortNames;

void Poller::add_action( Poller::Action action )
{
  /* the action won't be actually added until the next poll() function call.
     this allows us to call add_action inside the callback functions */
  action_add_queue_.push( action );
}

unsigned int Poller::Action::service_count( void ) const
{
  return direction == Direction::In ? fd.read_count() : fd.write_count();
}

Poller::Result Poller::poll( const int timeout_ms )
{
  /* first, let's add all the actions that are waiting in the queue */
  while ( not action_add_queue_.empty() ) {
    Action & action = action_add_queue_.front();
    pollfds_.push_back( { action.fd.fd_num(), 0, 0 } );
    actions_.emplace_back( move( action ) );
    action_add_queue_.pop();
  }

  assert( pollfds_.size() == actions_.size() );

  if ( timeout_ms == 0 ) {
    throw runtime_error( "poll asked to busy-wait" );
  }

  /* tell poll whether we care about each fd */
  auto it_action = actions_.begin();
  auto it_pollfd = pollfds_.begin();

  for ( ; it_action != actions_.end() and it_pollfd != pollfds_.end()
        ; it_action++, it_pollfd++ ) {
    assert( it_pollfd->fd == it_action->fd.fd_num() );
    it_pollfd->events = (it_action->active and it_action->when_interested())
      ? it_action->direction : 0;

    /* don't poll in on fds that have had EOF */
    if ( it_action->direction == Direction::In
       and it_action->fd.eof() ) {
      it_pollfd->events = 0;
    }
  }

  /* Quit if no member in pollfds_ has a non-zero direction */
  if ( not accumulate( pollfds_.begin(), pollfds_.end(), false,
                       [] ( bool acc, pollfd x ) { return acc or x.events; } ) ) {
    return Result::Type::Exit;
  }

  if ( 0 == CheckSystemCall( "poll", ::poll( &pollfds_[ 0 ], pollfds_.size(), timeout_ms ) ) ) {
    return Result::Type::Timeout;
  }

  it_action = actions_.begin();
  it_pollfd = pollfds_.begin();

  set<int> fds_to_remove;
  static int X = 0;
  X++;
  for ( ; it_action != actions_.end() and it_pollfd != pollfds_.end()
        ; it_action++, it_pollfd++ ) {
    if ( it_pollfd->revents & (POLLERR | POLLHUP | POLLNVAL) ) {
      if ( fds_to_remove.count( it_pollfd->fd ) == 0 ) {
        it_action->fderror_callback();
        fds_to_remove.insert( it_pollfd->fd );
      }
      continue;
    }

    if ( it_pollfd->revents & it_pollfd->events ) {
      /* we only want to call callback if revents includes
        the event we asked for */
      auto result = it_action->callback();

      switch ( result.result ) {
      case ResultType::Exit:
        return Result( Result::Type::Exit, result.exit_status );

      case ResultType::Cancel:
        it_action->active = false;
        break;

      case ResultType::CancelAll:
        fds_to_remove.insert( it_pollfd->fd );
        break;

      case ResultType::Continue:
        break;
      }
    }
  }

  remove_actions( fds_to_remove );

  return Result::Type::Success;
}

void Poller::remove_actions( const set<int> & fd_nums )
{
  if ( fd_nums.size() == 0 ) {
    return;
  }

  auto it_action = actions_.begin();
  auto it_pollfd = pollfds_.begin();

  while ( it_action != actions_.end() and it_pollfd != pollfds_.end() ) {
    if ( fd_nums.count( it_pollfd->fd ) ) {
      it_action = actions_.erase( it_action );
      it_pollfd = pollfds_.erase( it_pollfd );
    }
    else {
      it_action++;
      it_pollfd++;
    }
  }
}
