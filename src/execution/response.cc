/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "response.hh"

#include <iostream>
#include <stdexcept>
#include <google/protobuf/util/json_util.h>

#include "protobufs/gg.pb.h"

using namespace std;
using namespace gg;
using namespace google::protobuf::util;

ExecutionResponse ExecutionResponse::parse_message( const std::string & message )
{
  ExecutionResponse response;

  JsonParseOptions parse_options;
  gg::protobuf::ExecutionResponse response_proto;

  if ( not JsonStringToMessage( message, &response_proto ).ok() ) {
    cerr << "invalid response: " << message << endl;
    response.status = JobStatus::OperationalFailure;
    return response;
  }

  response.status = static_cast<JobStatus>( response_proto.return_code() );

  if ( response.status != JobStatus::Success ) {
    return response;
  }

  /*
  if ( response_proto.executed_thunks_size() != 1 ) {
    throw runtime_error( "current implementation only supports one thunk execution per response" );
  }
  */
  for ( const auto & exec_thunks : response_proto.executed_thunks() ) {
    response.thunk_hash.push_back( exec_thunks.thunk_hash() );

    for ( const auto & output_proto : exec_thunks.outputs() ) {
      vector<Output> next_output;
      next_output.push_back( { output_proto.tag(),
                               output_proto.hash(),
                               output_proto.size(),
                               output_proto.executable(),
                               output_proto.data() } );
      response.outputs.push_back( next_output );
    }
  }

  // response.thunk_hash = response_proto.executed_thunks( 0 ).thunk_hash();
  response.stdout = response_proto.stdout();

  return response;
}
