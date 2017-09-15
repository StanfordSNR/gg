/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "remote.hh"

#include <string>
#include <sstream>
#include <crypto++/base64.h>
#include <crypto++/files.h>
#include <cajun/json/reader.h>
#include <cajun/json/writer.h>
#include <cajun/json/elements.h>

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
  string base64_thunk;

  FileSource s( gg::paths::blob_path( thunk_hash ).string().c_str(), true,
                new Base64Encoder( new StringSink( base64_thunk ), false ) );

  json::Object lambda_event;
  lambda_event[ "thunk_hash" ] = json::String( thunk_hash );
  lambda_event[ "s3_bucket" ] = json::String( gg::remote::s3_bucket() );
  lambda_event[ "thunk_data" ] = json::String( base64_thunk );

  if ( timelog ) {
    lambda_event[ "timelog" ] = json::Boolean( true );
  }

  json::Array lambda_event_infiles;

  for ( const InFile & infile : thunk.infiles() ) {
    if ( infile.type() == InFile::Type::DUMMY_DIRECTORY ) {
      continue;
    }

    json::Object event_infile;
    event_infile[ "hash" ] = json::String( infile.content_hash() );
    event_infile[ "size" ] = json::Number( infile.size() );
    event_infile[ "executable" ] = json::Boolean(  infile.type() == InFile::Type::EXECUTABLE );

    lambda_event_infiles.Insert( event_infile );
  }

  lambda_event[ "infiles" ] = lambda_event_infiles;

  ostringstream oss;
  json::Writer::Write( lambda_event, oss );
  const string request_payload = oss.str();

  return LambdaInvocationRequest (
    credentials_, region_, function_name, request_payload,
    LambdaInvocationRequest::InvocationType::REQUEST_RESPONSE,
    LambdaInvocationRequest::LogType::NONE
  ).to_http_request();
}

ExecutionConnectionManager::ExecutionConnectionManager( const string & region )
  : address_( LambdaInvocationRequest::endpoint( region ), "https" )
{}

ConnectionContext & ExecutionConnectionManager::new_connection( const std::string & hash )
{
  if ( connections_.count( hash ) > 0 ) {
    throw runtime_error( "hash already exists" );
  }

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

  auto ret = connections_.emplace( make_pair( hash, move( lambda_socket ) ) );
  assert( ret.second );
  
  return ret.first->second;
}

void ExecutionConnectionManager::remove_connection( const std::string & hash )
{
  connections_.erase( hash );
}

RemoteResponse RemoteResponse::parse_message( const std::string & message )
{
  RemoteResponse response;

  istringstream iss { message };
  json::Object response_json;
  json::Reader::Read( response_json, iss );

  auto error_type_it = response_json.Find( "errorType" );
  if ( error_type_it != response_json.End() ) {
    /* Something happened */
    string error_type = static_cast<json::String>( error_type_it->element );

    if ( error_type == "GG-ExecutionFailed" ) {
      response.type = Type::EXECUTION_FAILURE;
      return response;
    }
    else {
      throw runtime_error( "unknown error type: " + error_type );
    }
  }

  response.type = Type::SUCCESS;
  response.thunk_hash = static_cast<json::String>( response_json[ "thunk_hash" ] );
  response.output_hash = static_cast<json::String>( response_json[ "output_hash" ] );
  response.output_size = static_cast<json::Number>( response_json[ "output_size" ] );
  response.is_executable = static_cast<json::Boolean>( response_json[ "executable_output" ] );

  return response;
}

RemoteResponse::RemoteResponse()
  : type(), thunk_hash(), output_hash(), output_size(), is_executable()
{}

void ConnectionContext::continue_SSL_connect()
{
  if ( state == State::needs_connect ) {
    socket.verify_no_errors();
    /* TCP successfully connected, so start SSL session */

    state = State::needs_ssl_write_to_connect;
  }

  if ( state == State::needs_ssl_write_to_connect
       or state == State::needs_ssl_read_to_connect ) {
    try {
      socket.connect();
    } catch ( const ssl_error & s ) {
      /* is it a WANT_READ or WANT_WRITE? */
      if ( s.error_code() == SSL_ERROR_WANT_READ ) {
        state = State::needs_ssl_read_to_connect;
        cerr << "wants read to connect\n";
      } else if ( s.error_code() == SSL_ERROR_WANT_WRITE ) {
        state = State::needs_ssl_write_to_connect;
        cerr << "wants write to connect\n";
      } else {
        cerr << "other ssl error: " << s.error_code() << endl;
        throw;
      }
    }

    state = State::ready;
    return;
  }

  assert( ready() );
  throw runtime_error( "session already connected");
}
