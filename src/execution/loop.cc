/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "loop.hh"

#include "net/http_response_parser.hh"
#include "thunk/ggutils.hh"
#include "util/exception.hh"
#include "util/optional.hh"

using namespace std;
using namespace PollerShortNames;

using ReductionResult = gg::cache::ReductionResult;

ExecutionLoop::ExecutionLoop()
  : signals_( { SIGCHLD, SIGCONT, SIGHUP, SIGTERM, SIGQUIT, SIGINT } ),
    signal_fd_( signals_ )
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

template<>
TCPConnectionContext &
ExecutionLoop::make_connection<UNSECURE>( const Address & address,
                                          const function<bool(string &&)> & data_callback,
                                          const function<void()> & error_callback,
                                          const function<void()> & close_callback )
{
  TCPSocket socket;
  socket.set_blocking( false );
  socket.connect_nonblock( address );

  auto connection_it = connection_contexts_.emplace( connection_contexts_.end(),
                                                     move( socket ) );

  auto fderror_callback =
    [connection_it, error_callback, close_callback, this]
    {
      error_callback();
      close_callback();
      connection_contexts_.erase( connection_it );
    };

  poller_.add_action(
    Poller::Action(
      connection_it->socket_, Direction::Out,
      [connection_it] ()
      {
        string::const_iterator last_write =
          connection_it->socket_.write( connection_it->write_buffer_.begin(),
                                        connection_it->write_buffer_.cend() );

        connection_it->write_buffer_.erase( 0, last_write - connection_it->write_buffer_.cbegin() );
        return ResultType::Continue;
      },
      [connection_it] { return connection_it->write_buffer_.size(); },
      fderror_callback
    )
  );

  poller_.add_action(
    Poller::Action(
      connection_it->socket_, Direction::In,
      [connection_it, data_callback { move( data_callback ) },
       close_callback { move( close_callback ) }, this] ()
      {
        if ( not data_callback( move( connection_it->socket_.read() ) ) ) {
          close_callback();
          connection_contexts_.erase( connection_it );
          return ResultType::CancelAll;
        }

        return ResultType::Continue;
      },
      [connection_it]() { return true; },
      fderror_callback
    )
  );

  return *connection_it;
}

template<>
SSLConnectionContext &
ExecutionLoop::make_connection<SECURE>( const Address & address,
                                        const function<bool(string &&)> & data_callback,
                                        const function<void()> & error_callback,
                                        const function<void()> & close_callback )
{
  TCPSocket socket;
  socket.set_blocking( false );
  socket.connect_nonblock( address );

  NBSecureSocket secure_socket { move( ssl_context_.new_secure_socket( move( socket ) ) ) };
  secure_socket.connect();

  auto connection_it = ssl_connection_contexts_.emplace( ssl_connection_contexts_.end(),
                                                         move( secure_socket ) );

  auto fderror_callback =
    [connection_it, error_callback, close_callback, this]
    {
      error_callback();
      close_callback();
      ssl_connection_contexts_.erase( connection_it );
    };

  poller_.add_action(
    Poller::Action(
      connection_it->socket_, Direction::Out,
      [connection_it] ()
      {
        connection_it->socket_.ezwrite( move( connection_it->write_buffer_ ) );
        connection_it->write_buffer_ = string {};
        return ResultType::Continue;
      },
      [connection_it] { return connection_it->write_buffer_.size(); },
      fderror_callback
    )
  );

  poller_.add_action(
    Poller::Action(
      connection_it->socket_, Direction::In,
      [connection_it, data_callback, this] ()
      {
        if ( not data_callback( move( connection_it->socket_.ezread() ) ) ) {
          ssl_connection_contexts_.erase( connection_it );
          return ResultType::CancelAll;
        }

        return ResultType::Continue;
      },
      [connection_it]() { return true; },
      fderror_callback
    )
  );

  return *connection_it;
}

template<ConnectionType is_secure>
uint64_t ExecutionLoop::make_http_request( const string & tag,
                                           const Address & address,
                                           const HTTPRequest & request,
                                           HTTPResponseCallbackFunc response_callback,
                                           FailureCallbackFunc failure_callback )
{
  const uint64_t connection_id = current_id_++;

  auto parser_it = http_response_parsers_.emplace( http_response_parsers_.end() );
  parser_it->new_request_arrived( request );

  auto data_callback =
    [connection_id, tag, parser_it, response_callback]
    ( string && data )
    {
      parser_it->parse( data );

      if ( not parser_it->empty() ) {
        response_callback( connection_id, tag, parser_it->front() );
        parser_it->pop();
        return false;
      }

      return true;
    };

  auto error_callback =
    [connection_id, tag, failure_callback]
    { failure_callback( connection_id, tag ); };

  auto close_callback =
    [parser_it, this]
    {
      http_response_parsers_.erase( parser_it );
    };

  auto & ctx = make_connection<is_secure>( address,
                                           data_callback,
                                           error_callback,
                                           close_callback );

  ctx.write_buffer_ = move( request.str() );

  return connection_id;
}

uint64_t ExecutionLoop::add_child_process( const string & tag,
                                           LocalCallbackFunc callback,
                                           FailureCallbackFunc /* failure_callback */,
                                           function<int()> && child_procedure )
{
  child_processes_.emplace_back( current_id_, callback, ChildProcess( tag, move( child_procedure ) ) );
  return current_id_++;
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

template
uint64_t ExecutionLoop::make_http_request<UNSECURE>( const string &,
                                                     const Address &,
                                                     const HTTPRequest &,
                                                     HTTPResponseCallbackFunc,
                                                     FailureCallbackFunc );

template
uint64_t ExecutionLoop::make_http_request<SECURE>( const string &,
                                                   const Address &,
                                                   const HTTPRequest &,
                                                   HTTPResponseCallbackFunc,
                                                   FailureCallbackFunc );
