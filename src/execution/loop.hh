/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef LOOP_HH
#define LOOP_HH

#include <list>
#include <vector>
#include <functional>
#include <unordered_map>

#include "socket.hh"
#include "secure_socket.hh"
#include "connection_context.hh"
#include "signalfd.hh"
#include "child_process.hh"
#include "signalfd.hh"
#include "poller.hh"

class ExecutionLoop
{
public:
  typedef std::function<void( const uint64_t /* id */,
                              const std::string & /* tag */ )> LocalCallbackFunc;
  typedef std::function<void( const uint64_t id /* id */,
                              const std::string & /* tag */,
                              const HTTPResponse & )> RemoteCallbackFunc;
  typedef std::function<void( const uint64_t /* id */,
                              const std::string & /* tag */ )> FailureCallbackFunc;

private:
  uint64_t current_id_{ 0 };

  SignalMask signals_;
  SignalFD signal_fd_;

  Poller poller_;
  std::list<std::tuple<uint64_t, LocalCallbackFunc, ChildProcess>> child_processes_;
  std::list<ConnectionContext> connection_contexts_;
  std::list<SSLConnectionContext> ssl_connection_contexts_;

  Poller::Action::Result handle_signal( const signalfd_siginfo & );

public:
  ExecutionLoop();

  /* the add_* functions will return a 64-bit number as a unique id */

  uint64_t add_child_process( const std::string & tag,
                              LocalCallbackFunc callback,
                              FailureCallbackFunc failure_callback,
                              std::function<int()> && child_procedure );

  template<class SocketType>
  uint64_t add_connection( const std::string & tag,
                           RemoteCallbackFunc callback,
                           FailureCallbackFunc failure_callback,
                           SocketType & socket,
                           const HTTPRequest & request );

  Poller::Result loop_once();
};

#endif /* LOOP_HH */
