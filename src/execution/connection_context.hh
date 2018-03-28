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
  std::string request_str {};
  std::string::const_iterator last_write {};

  bool something_to_write { true };

  ConnectionContext( SocketType && sock, const HTTPRequest & request )
    : socket( std::move( sock ) ), request_str( request.str() ),
      last_write( request_str.cbegin() )
  {
    responses.new_request_arrived( request );
  }
};

#endif /* CONNECTION_CONTEXT_HH */
