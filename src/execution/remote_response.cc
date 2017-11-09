/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "remote_response.hh"

#include <stdexcept>

#include <cajun/json/reader.h>
#include <cajun/json/writer.h>
#include <cajun/json/elements.h>

using namespace std;

RemoteResponse RemoteResponse::parse_message( const std::string & message,
                                              const bool wsk_output )
{
  RemoteResponse response;

  istringstream iss { message };
  json::Object response_json;
  json::Reader::Read( response_json, iss );

  if ( wsk_output ) {
    response.wsk_data.initialize();
    response.wsk_data->duration = static_cast<json::Number>( response_json[ "duration" ] );
    response.wsk_data->fn_name = static_cast<json::String>( response_json[ "name" ] );
    response_json = response_json[ "response" ][ "result" ];
  }

  auto error_type_it = response_json.Find( "errorType" );
  if ( error_type_it != response_json.End() ) {
    /* Something happened */
    string error_type = static_cast<json::String>( error_type_it->element );

    if ( error_type == "GG-ExecutionFailed" ) {
      response.type = Type::EXECUTION_FAILURE;
      return response;
    }
    else {
      cerr << message << endl;
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
