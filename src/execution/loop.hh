/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef LOOP_HH
#define LOOP_HH

#include <list>
#include <vector>
#include <functional>
#include <unordered_map>
#include <type_traits>

#include "connection_context.hh"
#include "net/http_response.hh"
#include "net/http_response_parser.hh"
#include "net/socket.hh"
#include "net/nb_secure_socket.hh"
#include "util/signalfd.hh"
#include "util/child_process.hh"
#include "util/poller.hh"

enum ConnectionType : bool { SECURE = true, UNSECURE = false };

class ExecutionLoop
{
public:
  typedef std::function<void( const uint64_t /* id */,
                              const std::string & /* tag */ )> LocalCallbackFunc;
  typedef std::function<bool( const uint64_t id /* id */,
                              const std::string & /* tag */,
                              const HTTPResponse & )> HTTPResponseCallbackFunc;
  typedef std::function<void( const uint64_t /* id */,
                              const std::string & /* tag */ )> FailureCallbackFunc;

private:
  uint64_t current_id_{ 0 };

  SignalMask signals_;
  SignalFD signal_fd_;

  Poller poller_ {};
  std::list<std::tuple<uint64_t, LocalCallbackFunc, ChildProcess>> child_processes_ {};
  std::list<TCPConnectionContext> connection_contexts_ {};
  std::list<SSLConnectionContext> ssl_connection_contexts_ {};

  std::list<HTTPResponseParser> http_response_parsers_ {};

  SSLContext ssl_context_ {};

  Poller::Action::Result handle_signal( const signalfd_siginfo & );

public:
  typedef decltype( connection_contexts_ )::iterator HTTPConnectionIterator;
  typedef decltype( ssl_connection_contexts_ )::iterator HTTPSConnectionIterator;

  ExecutionLoop();

  /* the add_* functions will return a 64-bit number as a unique id */

  uint64_t add_child_process( const std::string & tag,
                              LocalCallbackFunc callback,
                              FailureCallbackFunc failure_callback,
                              std::function<int()> && child_procedure );

  template<ConnectionType is_secure>
  typename std::conditional<is_secure, SSLConnectionContext &, TCPConnectionContext &>::type
  make_connection( const Address & address,
                   const std::function<bool(std::string &&)> & data_callback,
                   const std::function<void()> & error_callback = [](){},
                   const std::function<void()> & close_callback = [](){} );

  template<ConnectionType is_secure>
  uint64_t make_http_request( const std::string & tag,
                              const Address & address,
                              const HTTPRequest & request,
                              HTTPResponseCallbackFunc response_callback,
                              FailureCallbackFunc failure_callback );

  uint64_t make_listener( const Address & address,
                          const std::function<bool(TCPSocket &&)> & connection_callback );

  Poller & poller() { return poller_; }
  Poller::Result loop_once( const int timeout_ms = -1 );
};

#endif /* LOOP_HH */
