/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef CONNECTION_CONTEXT_HH
#define CONNECTION_CONTEXT_HH

#include <string>

#include "net/http_request.hh"
#include "net/http_response_parser.hh"
#include "net/secure_socket.hh"

template<class SocketType>
struct ConnectionContext
{
  SocketType socket;
  HTTPResponseParser responses {};
  std::string write_buffer {};

  ConnectionContext( SocketType && sock )
    : socket( std::move( sock ) )
  {}
};

#endif /* CONNECTION_CONTEXT_HH */
