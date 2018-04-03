/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef CONNECTION_CONTEXT_HH
#define CONNECTION_CONTEXT_HH

#include <string>
#include <queue>
#include <iostream>

#include "loop.hh"
#include "net/socket.hh"
#include "net/nb_secure_socket.hh"

template<class SocketType>
class ConnectionContext
{
  friend class ExecutionLoop;

private:
  SocketType socket_;
  std::string write_buffer_ {};

public:
  ConnectionContext( SocketType && sock )
    : socket_( std::move( sock ) )
  {}

  ~ConnectionContext()
  {
    if ( write_buffer_.size() ) {
      std::cerr << "ConnectionContext destroyed with data left in write buffer" << std::endl;
    }
  }

  void enqueue_write( const std::string & str ) { write_buffer_.append( str ); }
};

using TCPConnectionContext = ConnectionContext<TCPSocket>;
using SSLConnectionContext = ConnectionContext<NBSecureSocket>;

#endif /* CONNECTION_CONTEXT_HH */
