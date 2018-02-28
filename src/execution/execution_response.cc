/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "execution_response.hh"

#include <iostream>
#include <stdexcept>
#include <google/protobuf/util/json_util.h>

#include "gg.pb.h"

using namespace std;
using namespace gg;
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

  if ( execution_response_proto.executed_thunks_size() != 1 ) {
    throw runtime_error( "current implementation only supports one thunk execution per response" );
  }

  response.thunk_hash = execution_response_proto.executed_thunks( 0 ).thunk_hash();
  response.output_hash = execution_response_proto.executed_thunks( 0 ).output_hash();
  response.output_size = execution_response_proto.executed_thunks( 0 ).output_size();
  response.is_executable = execution_response_proto.executed_thunks( 0 ).executable_output();

  return response;
}

ExecutionResponse::ExecutionResponse()
  : status(), thunk_hash(), output_hash(), output_size(),
    is_executable(), output()
{}
