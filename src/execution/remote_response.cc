/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "remote_response.hh"

#include <stdexcept>
#include <google/protobuf/util/json_util.h>

#include "gg.pb.h"

using namespace std;
using namespace google::protobuf::util;

RemoteResponse RemoteResponse::parse_message( const std::string & message )
{
  JsonParseOptions parse_options;
  gg::protobuf::ExecutionResponse execution_response_proto;
  if ( not JsonStringToMessage( message, &execution_response_proto ).ok() ) {
    throw runtime_error( "could not parse json response" );
  }

  RemoteResponse response;

  if ( execution_response_proto.output().length() ) {
    response.output.reset( execution_response_proto.output() );
  }

  if ( execution_response_proto.return_code() ) {
    return response;
  }

  response.status = JobStatus::Success;
  response.thunk_hash = execution_response_proto.thunk_hash();
  response.output_hash = execution_response_proto.output_hash();
  response.output_size = execution_response_proto.output_size();
  response.is_executable = execution_response_proto.executable_output();

  return response;
}

RemoteResponse::RemoteResponse()
  : status(), thunk_hash(), output_hash(), output_size(),
    is_executable(), output( false )
{}
