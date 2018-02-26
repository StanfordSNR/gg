/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "execution_response.hh"

#include <iostream>
#include <stdexcept>
#include <google/protobuf/util/json_util.h>

#include "gg.pb.h"

using namespace std;
using namespace google::protobuf::util;

ExecutionResponse ExecutionResponse::parse_message( const std::string & message )
{
  ExecutionResponse response;

  JsonParseOptions parse_options;
  gg::protobuf::ExecutionResponse execution_response_proto;

  if ( not JsonStringToMessage( message, &execution_response_proto ).ok() ) {
    cerr << "invalid response: " << message << endl;
    response.status = JobStatus::OperationalFailure;
    return response;
  }

  response.status = static_cast<JobStatus>( execution_response_proto.return_code() );
  response.output = execution_response_proto.output();

  if ( response.status != JobStatus::Success ) {
    return response;
  }

  response.thunk_hash = execution_response_proto.thunk_hash();
  response.output_hash = execution_response_proto.output_hash();
  response.output_size = execution_response_proto.output_size();
  response.is_executable = execution_response_proto.executable_output();

  return response;
}

ExecutionResponse::ExecutionResponse()
  : status(), thunk_hash(), output_hash(), output_size(),
    is_executable(), output()
{}
