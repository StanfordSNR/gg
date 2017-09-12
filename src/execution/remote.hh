/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef REMOTE_HH
#define REMOTE_HH

#include <string>
#include <unordered_map>

#include "thunk.hh"
#include "lambda.hh"
#include "http_request.hh"
#include "aws.hh"
#include "secure_socket.hh"
#include "address.hh"

namespace lambda {

  class RequestGenerator
  {
  private:
    AWSCredentials credentials_;
    std::string region_;

  public:
    RequestGenerator( const AWSCredentials & credentials, const std::string & region );
    HTTPRequest generate( const gg::thunk::Thunk & thunk, const std::string & thunk_hash );
  };

  class ExecutionSocketManager
  {
  private:
    SSLContext ssl_context_ {};
    Address address_;

    /* thunk_hash -> socket */
    std::unordered_map<std::string, SecureSocket> sockets_ {};

  public:
    ExecutionSocketManager( const std::string & region );
    SecureSocket & new_socket( const std::string & hash );
  };

}


#endif /* REMOTE_HH */
