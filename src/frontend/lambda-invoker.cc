/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <iostream>
#include <string>

#include "lambda.hh"
#include "socket.hh"
#include "secure_socket.hh"
#include "http_request.hh"
#include "http_response_parser.hh"

using namespace std;

int main()
{
  const string region = "us-west-1";
  const string function_name = "hello-world";
  const string endpoint = "lambda.us-west-1.amazonaws.com";

  LambdaInvocationRequest lambda_request { {}, region, function_name, "{}" };

  TCPSocket sock;
  const Address address { endpoint, "https" };
  sock.connect( address );

  SSLContext ssl_context;
  HTTPResponseParser responses;
  SecureSocket lambda = ssl_context.new_secure_socket( move( sock ) );
  lambda.connect();

  HTTPRequest outgoing_request = lambda_request.to_http_request();
  responses.new_request_arrived( outgoing_request );

  lambda.write( outgoing_request.str() );

  responses.parse( lambda.read() );

  if ( not responses.empty() ) {
      cout << responses.front().str() << endl;
  }

  return 0;
}
