/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "loop.hh"

#include "ggpaths.hh"
#include "exception.hh"
#include "optional.hh"

using namespace std;
using namespace PollerShortNames;

using ReductionResult = gg::cache::ReductionResult;
using SSLConnectionState = SSLConnectionContext::State;

ExecutionLoop::ExecutionLoop( function<void( const HTTPResponse & )> remote_callback,
                              function<void( const std::string &, const std::string & )> local_callback)
  : signals_( { SIGCHLD, SIGCONT, SIGHUP, SIGTERM, SIGQUIT, SIGINT } ),
    poller_(), child_processes_(), connection_contexts_(),
    ssl_connection_contexts_(),
    remote_finish_callback( remote_callback ),
    local_finish_callback( local_callback )
{
  signals_.set_as_mask();
}

template <typename... Targs>
void ExecutionLoop::add_child_process( Targs && ... Fargs )
{
  child_processes_.emplace_back( std::forward<Targs>( Fargs )... );
}

void ExecutionLoop::add_connection( const string & tag, TCPSocket && socket,
                                    const HTTPRequest & request )
{
  /* XXX not thread-safe */
  connection_contexts_.emplace( piecewise_construct,
                                forward_as_tuple( tag ),
                                forward_as_tuple( move( socket ), request ) );

  ConnectionContext & connection = connection_contexts_.at( tag );

  poller_.add_action(
    Poller::Action(
      connection.socket, Direction::Out,
      [&connection] ()
      {
        connection.socket.verify_no_errors();

        if ( connection.state == ConnectionContext::State::needs_connect ) {
          connection.state = ConnectionContext::State::ready;
        }

        connection.last_write = connection.socket.write( connection.last_write,
                                                         connection.request_str.cend() );

        if ( connection.last_write == connection.request_str.cend() ) {
          connection.something_to_write = false;
        }

        return ResultType::Continue;
      },
      [&connection] { return connection.something_to_write; }
    )
  );

  poller_.add_action(
    Poller::Action(
      connection.socket, Direction::In,
      [&connection, this] ()
      {
        connection.responses.parse( connection.socket.read() );

        if ( not connection.responses.empty() ) {
          remote_finish_callback( connection.responses.front() );
        }

        return ResultType::Continue;
      },
      [&connection]() { return connection.ready(); }
    )
  );
}

void ExecutionLoop::add_connection( const string & tag, SecureSocket && socket,
                                    HTTPRequest && request )
{
  /* XXX not thread-safe */
  ssl_connection_contexts_.emplace( piecewise_construct,
                                    forward_as_tuple( tag ),
                                    forward_as_tuple( move( socket ), request ) );

  SSLConnectionContext & connection = ssl_connection_contexts_.at( tag );

  poller_.add_action(
    Poller::Action(
      connection.socket, Direction::Out,
      [&connection]()
      {
        /* did it connect successfully? */
        if ( not connection.connected() ) {
          connection.continue_SSL_connect();
        }
        else if ( connection.state == SSLConnectionState::needs_ssl_write_to_write or
             ( connection.state == SSLConnectionState::ready and connection.something_to_write ) ) {
          connection.continue_SSL_write();
        }
        else if ( connection.state == SSLConnectionState::needs_ssl_write_to_read ) {
          connection.continue_SSL_read();
        }

        return ResultType::Continue;
      },
      [&connection]()
      {
        return ( connection.state == SSLConnectionState::needs_connect ) or
               ( connection.state == SSLConnectionState::needs_ssl_write_to_connect ) or
               ( connection.state == SSLConnectionState::needs_ssl_write_to_write ) or
               ( connection.state == SSLConnectionState::needs_ssl_write_to_read ) or
               ( connection.state == SSLConnectionState::ready and connection.something_to_write );
      }
    )
  );

  /* what to do when socket is readable */
  poller_.add_action(
    Poller::Action(
      connection.socket, Direction::In,
      [&connection, this]()
      {
        if ( not connection.connected() ) {
          connection.continue_SSL_connect();
        }
        else if ( connection.state == SSLConnectionState::needs_ssl_read_to_write ) {
          connection.continue_SSL_write();
        }
        else if ( connection.state == SSLConnectionState::needs_ssl_read_to_read or
                  connection.state == SSLConnectionState::ready ) {
          connection.continue_SSL_read();
        }

        if ( not connection.responses.empty() ) {
          remote_finish_callback( connection.responses.front() );
        }

        return ResultType::Continue;
      },
      [&connection]()
      {
        return ( connection.state == SSLConnectionState::needs_connect ) or
               ( connection.state == SSLConnectionState::needs_ssl_read_to_connect ) or
               ( connection.state == SSLConnectionState::needs_ssl_read_to_write ) or
               ( connection.state == SSLConnectionState::needs_ssl_read_to_read ) or
               ( connection.state == SSLConnectionState::ready );
      }
    )
  );
}

Poller::Action::Result ExecutionLoop::handle_signal( const signalfd_siginfo & sig )
{
  switch ( sig.ssi_signo ) {
  case SIGCONT:
    for ( auto & child : child_processes_ ) {
      child.resume();
    }
    break;

  case SIGCHLD:
    if ( child_processes_.empty() ) {
      throw runtime_error( "received SIGCHLD without any managed children" );
    }

    for ( auto it = child_processes_.begin(); it != child_processes_.end(); it++ ) {
      ChildProcess & child = *it;

      if ( child.terminated() or ( not child.waitable() ) ) {
        continue;
      }

      child.wait( true );

      if ( child.terminated() ) {
        if ( child.exit_status() != 0 ) {
          child.throw_exception();
        }

        /* Update the dependency graph now that we know this process ended
        with exit code 0. */
        const string & thunk_hash = child.name();
        Optional<ReductionResult> result = gg::cache::check( thunk_hash );

        if ( not result.initialized() or result->order != 0 ) {
          throw runtime_error( "could not find the reduction entry" );
        }

        local_finish_callback( thunk_hash, result->hash );

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
