/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef CONNECTION_CONTEXT_HH
#define CONNECTION_CONTEXT_HH

#include <string>

#include "secure_socket.hh"
#include "http_request.hh"
#include "http_response_parser.hh"

struct SSLConnectionContext
{
  enum class State { needs_connect,
                     needs_ssl_read_to_connect,
                     needs_ssl_write_to_connect,
                     needs_ssl_write_to_write,
                     needs_ssl_write_to_read,
                     needs_ssl_read_to_write,
                     needs_ssl_read_to_read,
                     ready,
                     closed };

  State state { State::needs_connect };

  SecureSocket socket;
  HTTPResponseParser responses {};
  std::string request_str {};

  bool something_to_write { true };

  SSLConnectionContext( SecureSocket && sock, const HTTPRequest & request )
    : socket( std::move( sock ) ), request_str( request.str() )
  {
    responses.new_request_arrived( request );
  }

  bool ready() const { return state == State::ready; }

  bool connected() const
  {
    return ( state != State::needs_connect ) and
           ( state != State::needs_ssl_read_to_connect ) and
           ( state != State::needs_ssl_write_to_connect );
  }

  void continue_SSL_connect();
  void continue_SSL_write();
  void continue_SSL_read();
};

#endif /* CONNECTION_CONTEXT_HH */
