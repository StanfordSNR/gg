/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef REMOTE_HH
#define REMOTE_HH

#include <sys/types.h>
#include <string>
#include <unordered_map>

#include "thunk.hh"
#include "lambda.hh"
#include "http_request.hh"
#include "http_response_parser.hh"
#include "aws.hh"
#include "secure_socket.hh"
#include "address.hh"

class RemoteResponse
{
private:
  RemoteResponse();
  
public:
  enum class Type
  {
    SUCCESS,
    EXECUTION_FAILURE,
    LAMBDA_FAILURE,
    RATE_LIMIT
  } type;

  std::string thunk_hash;
  std::string output_hash;
  off_t output_size;
  bool is_executable;

  static RemoteResponse parse_message( const std::string & message );
};

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

  class ExecutionConnectionManager
  {
  private:
    SSLContext ssl_context_ {};
    Address address_;

    /* thunk_hash -> socket */
    std::unordered_map<std::string, SecureSocket> sockets_ {};
    std::unordered_map<std::string, HTTPResponseParser> responses_ {};

  public:
    ExecutionConnectionManager( const std::string & region );
    SecureSocket & new_connection( const std::string & hash );
    void remove_connection( const std::string & hash );

    HTTPResponseParser & response_parser( const std::string & hash ) { return responses_.at( hash ); }
  };

}

#endif /* REMOTE_HH */
