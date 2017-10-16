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
#include "poller.hh"

class ExecutionLoop
{
private:
  SignalMask signals_;
  Poller poller_;
  std::list<ChildProcess> child_processes_;
  std::unordered_map<std::string, ConnectionContext> connection_contexts_;
  std::unordered_map<std::string, SSLConnectionContext> ssl_connection_contexts_;

  Poller::Action::Result handle_signal( const signalfd_siginfo & );

  std::function<void( const HTTPResponse & )> remote_finish_callback {};
  std::function<void( const std::string &, const std::string & )> local_finish_callback {};

public:
  ExecutionLoop();

  template <typename... Targs>
  void add_child_process( Targs && ... Fargs );

  void add_connection( const std::string & tag, TCPSocket && socket,
                       const HTTPRequest & request );

  void add_connection( const std::string & tag, SecureSocket && socket,
                       HTTPRequest && request );

  void loop_once();
};

#endif /* LOOP_HH */
