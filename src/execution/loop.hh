/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef LOOP_HH
#define LOOP_HH

#include <list>
#include <vector>
#include <functional>
#include <unordered_map>

#include "connection_context.hh"
#include "net/socket.hh"
#include "net/nb_secure_socket.hh"
#include "util/signalfd.hh"
#include "util/child_process.hh"
#include "util/poller.hh"

class ExecutionLoop
{
public:
  typedef std::function<void( const uint64_t /* id */,
                              const std::string & /* tag */ )> LocalCallbackFunc;
  typedef std::function<bool( const uint64_t id /* id */,
                              const std::string & /* tag */,
                              const HTTPResponse & )> RemoteCallbackFunc;
  typedef std::function<void( const uint64_t /* id */,
                              const std::string & /* tag */ )> FailureCallbackFunc;

private:
  uint64_t current_id_{ 0 };

  SignalMask signals_;
  SignalFD signal_fd_;

  Poller poller_ {};
  std::list<std::tuple<uint64_t, LocalCallbackFunc, ChildProcess>> child_processes_ {};
  std::list<ConnectionContext<TCPSocket>> connection_contexts_ {};
  std::list<ConnectionContext<NBSecureSocket>> ssl_connection_contexts_ {};

  Poller::Action::Result handle_signal( const signalfd_siginfo & );

public:
  typedef decltype( connection_contexts_ )::iterator ConnectionIterator;
  typedef decltype( ssl_connection_contexts_ )::iterator SSLConnectionIterator;

  ExecutionLoop();

  /* the add_* functions will return a 64-bit number as a unique id */

  uint64_t add_child_process( const std::string & tag,
                              LocalCallbackFunc callback,
                              FailureCallbackFunc failure_callback,
                              std::function<int()> && child_procedure );

  template<class SocketType>
  std::pair<uint64_t, typename std::list<ConnectionContext<SocketType>>::iterator>
  add_connection( SocketType && socket,
                  const std::string & tag,
                  RemoteCallbackFunc callback,
                  FailureCallbackFunc failure_callback );

  Poller & poller() { return poller_; }
  Poller::Result loop_once( const int timeout_ms = -1 );
};

#endif /* LOOP_HH */
