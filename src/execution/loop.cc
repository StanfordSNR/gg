/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "loop.hh"

#include "thunk/ggutils.hh"
#include "util/exception.hh"
#include "util/optional.hh"

using namespace std;
using namespace PollerShortNames;

using ReductionResult = gg::cache::ReductionResult;
using SSLConnectionState = SSLConnectionContext::State;

ExecutionLoop::ExecutionLoop()
  : signals_( { SIGCHLD, SIGCONT, SIGHUP, SIGTERM, SIGQUIT, SIGINT } ),
    signal_fd_( signals_ ),
    poller_(), child_processes_(), connection_contexts_(),
    ssl_connection_contexts_()
{
  signals_.set_as_mask();

  poller_.add_action(
    Poller::Action(
      signal_fd_.fd(), Direction::In,
      [&]() { return handle_signal( signal_fd_.read_signal() ); },
      [&]() { return ( child_processes_.size() > 0 or
                       connection_contexts_.size() > 0 or
                       ssl_connection_contexts_.size() > 0 ); }
    )
  );
}

Poller::Result ExecutionLoop::loop_once( const int timeout_ms )
{
  return poller_.poll( timeout_ms );
}

uint64_t ExecutionLoop::add_child_process( const string & tag,
                                           LocalCallbackFunc callback,
                                           FailureCallbackFunc /* failure_callback */,
                                           std::function<int()> && child_procedure )
{
  child_processes_.emplace_back( current_id_, callback, ChildProcess( tag, move( child_procedure ) ) );
  return current_id_++;
}

template<>
uint64_t ExecutionLoop::add_connection( const string & tag,
                                        RemoteCallbackFunc callback,
                                        FailureCallbackFunc failure_callback,
                                        TCPSocket & socket,
                                        const HTTPRequest & request )
{
  /* XXX not thread-safe */
  const uint64_t connection_id = current_id_++;
  auto connection_it = connection_contexts_.emplace( connection_contexts_.end(),
                                                     move( socket ), request );

  poller_.add_action(
    Poller::Action(
      connection_it->socket, Direction::Out,
      [connection_it] ()
      {
        connection_it->socket.verify_no_errors();

        if ( connection_it->state == ConnectionContext::State::needs_connect ) {
          connection_it->state = ConnectionContext::State::ready;
        }

        connection_it->last_write = connection_it->socket.write( connection_it->last_write,
                                                                 connection_it->request_str.cend() );

        if ( connection_it->last_write == connection_it->request_str.cend() ) {
          connection_it->something_to_write = false;
        }

        return ResultType::Continue;
      },
      [connection_it] { return connection_it->something_to_write; },
      [connection_id, tag, failure_callback] { failure_callback( connection_id, tag ); }
    )
  );

  poller_.add_action(
    Poller::Action(
      connection_it->socket, Direction::In,
      [connection_it, tag, callback, connection_id, this] ()
      {
        connection_it->responses.parse( connection_it->socket.read() );

        if ( not connection_it->responses.empty() ) {
          connection_it->state = ConnectionContext::State::closed;
          callback( connection_id, tag, connection_it->responses.front() );
          connection_contexts_.erase( connection_it );
          return ResultType::CancelAll;
        }

        return ResultType::Continue;
      },
      [connection_it]() { return connection_it->ready(); },
      [connection_id, tag, failure_callback] { failure_callback( connection_id, tag ); }
    )
  );

  return connection_id;
}

template<>
uint64_t ExecutionLoop::add_connection( const string & tag,
                                        RemoteCallbackFunc callback,
                                        FailureCallbackFunc failure_callback,
                                        SecureSocket & socket,
                                        const HTTPRequest & request )
{
  /* XXX not thread-safe */
  const uint64_t connection_id = current_id_++;
  auto connection_it = ssl_connection_contexts_.emplace( ssl_connection_contexts_.end(),
                                                         move( socket ), request );

  poller_.add_action(
    Poller::Action(
      connection_it->socket, Direction::Out,
      [connection_it, connection_id, tag, failure_callback]()
      {
        try {
          /* did it connect successfully? */
          if ( not connection_it->connected() ) {
            connection_it->continue_SSL_connect();
          }
          else if ( connection_it->state == SSLConnectionState::needs_ssl_write_to_write or
                  ( connection_it->state == SSLConnectionState::ready and
                    connection_it->something_to_write ) ) {
            connection_it->continue_SSL_write();
          }
          else if ( connection_it->state == SSLConnectionState::needs_ssl_write_to_read ) {
            connection_it->continue_SSL_read();
          }
        }
        catch ( const ssl_error & ex ) {
          failure_callback( connection_id, tag );
          return ResultType::CancelAll;
        }

        return ResultType::Continue;
      },
      [connection_it]()
      {
        return ( connection_it->state == SSLConnectionState::needs_connect ) or
               ( connection_it->state == SSLConnectionState::needs_ssl_write_to_connect ) or
               ( connection_it->state == SSLConnectionState::needs_ssl_write_to_write ) or
               ( connection_it->state == SSLConnectionState::needs_ssl_write_to_read ) or
               ( connection_it->state == SSLConnectionState::ready and connection_it->something_to_write );
      },
      [connection_id, tag, failure_callback] { failure_callback( connection_id, tag ); }
    )
  );

  /* what to do when socket is readable */
  poller_.add_action(
    Poller::Action(
      connection_it->socket, Direction::In,
      [connection_it, tag, callback, failure_callback, connection_id, this]()
      {
        try {
          if ( not connection_it->connected() ) {
            connection_it->continue_SSL_connect();
          }
          else if ( connection_it->state == SSLConnectionState::needs_ssl_read_to_write ) {
            connection_it->continue_SSL_write();
          }
          else if ( connection_it->state == SSLConnectionState::needs_ssl_read_to_read or
                    connection_it->state == SSLConnectionState::ready ) {
            connection_it->continue_SSL_read();
          }
        }
        catch ( const ssl_error & error ) {
          failure_callback( connection_id, tag );
          return ResultType::CancelAll;
        }

        if ( not connection_it->responses.empty() ) {
          connection_it->state = SSLConnectionContext::State::closed;
          callback( connection_id, tag, connection_it->responses.front() );
          ssl_connection_contexts_.erase( connection_it );
          return ResultType::CancelAll;
        }

        return ResultType::Continue;
      },
      [connection_it]()
      {
        return ( connection_it->state == SSLConnectionState::needs_connect ) or
               ( connection_it->state == SSLConnectionState::needs_ssl_read_to_connect ) or
               ( connection_it->state == SSLConnectionState::needs_ssl_read_to_write ) or
               ( connection_it->state == SSLConnectionState::needs_ssl_read_to_read ) or
               ( connection_it->state == SSLConnectionState::ready );
      },
      [connection_id, tag, failure_callback] { failure_callback( connection_id, tag ); }
    )
  );

  return connection_id;
}

Poller::Action::Result ExecutionLoop::handle_signal( const signalfd_siginfo & sig )
{
  switch ( sig.ssi_signo ) {
  case SIGCONT:
    for ( auto & child : child_processes_ ) {
      get<2>( child ).resume();
    }
    break;

  case SIGCHLD:
    if ( child_processes_.empty() ) {
      throw runtime_error( "received SIGCHLD without any managed children" );
    }

    for ( auto it = child_processes_.begin(); it != child_processes_.end(); it++ ) {
      ChildProcess & child = get<2>( *it );

      if ( child.terminated() or ( not child.waitable() ) ) {
        continue;
      }

      child.wait( true );

      if ( child.terminated() ) {
        if ( child.exit_status() != 0 ) {
          child.throw_exception();
        }

        auto & callback = get<1>( *it );
        callback( get<0>( *it ), child.name() );

        it = child_processes_.erase( it );
        it--;
      }
      else if ( not child.running() ) {
        /* suspend parent too */
        CheckSystemCall( "raise", raise( SIGSTOP ) );
      }
    }

    break;

  case SIGHUP:
  case SIGTERM:
  case SIGQUIT:
  case SIGINT:
    throw runtime_error( "interrupted by signal" );

  default:
    throw runtime_error( "unknown signal" );
  }

  return ResultType::Continue;
}
