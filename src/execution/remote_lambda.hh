/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef REMOTE_LAMBDA_HH
#define REMOTE_LAMBDA_HH

#include <sys/types.h>
#include <string>
#include <unordered_map>

#include "connection_context.hh"
#include "thunk.hh"
#include "lambda.hh"
#include "http_request.hh"
#include "http_response_parser.hh"
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
    HTTPRequest generate( const gg::thunk::Thunk & thunk, const std::string & thunk_hash,
                          const bool timelog = true );
  };

  class ExecutionConnectionManager
  {
  private:
    SSLContext ssl_context_ {};
    Address address_;
    RequestGenerator request_generator_;

    /* thunk_hash -> socket */
    std::unordered_map<std::string, SSLConnectionContext> connections_ {};

  public:
    ExecutionConnectionManager( const AWSCredentials & credentials,
                                const std::string & region );

    SSLConnectionContext & new_connection( const gg::thunk::Thunk & thunk,
                                           const std::string & hash );

    SSLConnectionContext & connection_context( const std::string & hash ) { return connections_.at( hash ); }

    void remove_connection( const std::string & hash );
  };

}

#endif /* REMOTE_LAMBDA_HH */
