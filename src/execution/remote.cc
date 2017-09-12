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
                                        const string & thunk_hash )
{
  const string function_name = "gg-" + thunk.executable_hash();
  string base64_thunk;

  FileSource s( gg::paths::blob_path( thunk_hash ).string().c_str(), true,
                new Base64Encoder( new StringSink( base64_thunk ), false ) );

  json::Object lambda_event;
  lambda_event[ "thunk_hash" ] = json::String( thunk_hash );
  lambda_event[ "s3_bucket" ] = json::String( gg::remote::s3_bucket() );
  lambda_event[ "thunk_data" ] = json::String( base64_thunk );

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

SecureSocket & ExecutionConnectionManager::new_connection( const std::string & hash )
{
  if ( sockets_.count( hash ) > 0 ) {
    throw runtime_error( "hash already exists" );
  }

  TCPSocket sock;
  sock.connect( address_ );
  SecureSocket lambda_socket = ssl_context_.new_secure_socket( move( sock ) );
  lambda_socket.connect();

  sockets_.emplace( make_pair( hash, move( lambda_socket ) ) );

  responses_[ hash ];
  return sockets_.at( hash );
}

void ExecutionConnectionManager::remove_connection( const std::string & hash )
{
  sockets_.at( hash ).close();
  sockets_.erase( hash );
  responses_.erase( hash );
}

RemoteResponse::RemoteResponse( const string & response_str )
  : thunk_hash(), output_hash(), output_size(), is_executable()
{
  istringstream iss { response_str };
  json::Object response_json;
  json::Reader::Read( response_json, iss );

  thunk_hash = static_cast<json::String>( response_json[ "thunk_hash" ] );
  output_hash = static_cast<json::String>( response_json[ "output_hash" ] );
  output_size = static_cast<json::Number>( response_json[ "output_size" ] );
  is_executable = static_cast<json::Boolean>( response_json[ "executable_output" ] );
}
