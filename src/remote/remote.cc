/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "remote.hh"

#include <string>
#include <crypto++/base64.h>
#include <google/protobuf/util/json_util.h>

#include "digest.hh"
#include "thunk.hh"
#include "thunk_writer.hh"
#include "ggpaths.hh"
#include "gg.pb.h"

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

HTTPRequest RequestGenerator::generate( const Thunk & thunk )
{
  const string serialized_thunk = ThunkWriter::serialize_thunk( thunk );
  const string thunk_hash = digest::sha256( serialized_thunk );
  const string function_name = "gg-" + thunk.executable_hash();
  string base64_thunk;

  StringSource s( serialized_thunk, true,
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
