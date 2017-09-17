/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "remote.hh"

#include <string>
#include <sstream>

#include "digest.hh"
#include "thunk.hh"
#include "thunk_writer.hh"
#include "ggpaths.hh"
#include "gg.pb.h"
#include "lambda.hh"
#include "exception.hh"

using namespace std;
using namespace gg;
using namespace gg::thunk;
using namespace CryptoPP;
using namespace lambda;

RequestGenerator::RequestGenerator( const AWSCredentials & credentials,
                                    const string & region )
  : credentials_( credentials ), region_( region )
{}

HTTPRequest RequestGenerator::generate( const Thunk & thunk,
                                        const string & thunk_hash,
                                        const bool timelog )
{
  const string function_name = "gg-" + thunk.executable_hash();

  return LambdaInvocationRequest (
    credentials_, region_, function_name, thunk.execution_payload( thunk_hash, timelog ),
    LambdaInvocationRequest::InvocationType::REQUEST_RESPONSE,
    LambdaInvocationRequest::LogType::NONE
  ).to_http_request();
}

ExecutionConnectionManager::ExecutionConnectionManager( const AWSCredentials & credentials,
                                                        const string & region )
  : address_( LambdaInvocationRequest::endpoint( region ), "https" ),
    request_generator_( credentials, region )
{}

ConnectionContext & ExecutionConnectionManager::new_connection( const Thunk & thunk, const std::string & hash )
{
  if ( connections_.count( hash ) > 0 ) {
    throw runtime_error( "hash already exists" );
  }

  HTTPRequest request = request_generator_.generate( thunk, hash, true );

  TCPSocket sock;
  sock.set_blocking( false );
  try {
    sock.connect( address_ );
    throw runtime_error( "nonblocking connect unexpectedly succeeded immediately" );
  } catch ( const unix_error & e ) {
    if ( e.error_code() == EINPROGRESS ) {
      /* do nothing */
    } else {
      throw;
    }
  }

  SecureSocket lambda_socket = ssl_context_.new_secure_socket( move( sock ) );
  /* don't try to SSL_connect yet */

  auto ret = connections_.emplace( piecewise_construct,
                                  forward_as_tuple( hash ),
                                  forward_as_tuple( move( lambda_socket ), move ( request ) ) );
  assert( ret.second );

  return ret.first->second;
}

void ExecutionConnectionManager::remove_connection( const std::string & hash )
{
  connections_.erase( hash );
}
