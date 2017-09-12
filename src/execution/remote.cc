/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "remote.hh"

#include <string>
#include <crypto++/base64.h>
#include <crypto++/files.h>
#include <google/protobuf/util/json_util.h>

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
using namespace google::protobuf::util;
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

  gg::protobuf::LambdaEvent lambda_event;
  lambda_event.set_thunk_hash( thunk_hash );
  lambda_event.set_s3_bucket( gg::remote::s3_bucket() );
  lambda_event.set_thunk_data( base64_thunk );

  for ( const InFile & infile : thunk.infiles() ) {
    if ( infile.type() == InFile::Type::DUMMY_DIRECTORY ) {
      continue;
    }

    protobuf::LambdaInFile * proto_infile = lambda_event.add_infiles();
    proto_infile->set_hash( infile.content_hash() );
    proto_infile->set_size( infile.size() );
    proto_infile->set_executable( infile.type() == InFile::Type::EXECUTABLE );
  }

  string request_payload;

  JsonPrintOptions options;
  options.add_whitespace = false;
  options.always_print_primitive_fields = true;
  MessageToJsonString( lambda_event, &request_payload, options );

  return LambdaInvocationRequest (
    credentials_, region_, function_name, request_payload,
    LambdaInvocationRequest::InvocationType::REQUEST_RESPONSE,
    LambdaInvocationRequest::LogType::NONE
  ).to_http_request();
}

ExecutionConnectionManager::ExecutionConnectionManager( const string & region )
  : address_( LambdaInvocationRequest::endpoint( region ), "https" )
{}

SecureSocket & ExecutionConnectionManager::new_socket( const std::string & hash )
{
  if ( sockets_.count( hash ) > 0 ) {
    throw runtime_error( "hash already exists" );
  }

  TCPSocket sock;
  sock.connect( address_ );
  sockets_.emplace( make_pair( hash, move( ssl_context_.new_secure_socket( move( sock ) ) ) ) );

  return sockets_.at( hash );
}

RemoteResponse::RemoteResponse( const string & response_str )
  : thunk_hash(), output_hash(), output_size(), is_executable()
{
  protobuf::RemoteResponse rr_proto;

  JsonParseOptions options;
  options.ignore_unknown_fields = true;

  JsonStringToMessage( response_str, &rr_proto, options );

  thunk_hash = rr_proto.thunk_hash();
  output_hash = rr_proto.output_hash();
  output_size = rr_proto.output_size();
  is_executable = rr_proto.executable_output();
}
