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
  typedef std::function<void( const std::string & )> LocalCallbackFunc;
  typedef std::function<void( const std::string &, const HTTPResponse & )> RemoteCallbackFunc;

private:
  SignalMask signals_;
  SignalFD signal_fd_;

  Poller poller_;
  std::list<std::pair<LocalCallbackFunc, ChildProcess>> child_processes_;
  std::unordered_map<std::string, ConnectionContext> connection_contexts_;
  std::unordered_map<std::string, SSLConnectionContext> ssl_connection_contexts_;

  Poller::Action::Result handle_signal( const signalfd_siginfo & );

public:
  ExecutionLoop();

  void add_child_process( const std::string & tag, LocalCallbackFunc callback,
                          std::function<int()> && child_procedure );

  template<class SocketType>
  void add_connection( const std::string & tag, RemoteCallbackFunc callback,
                       SocketType && socket, const HTTPRequest & request );

  void loop_once();
};

#endif /* LOOP_HH */
