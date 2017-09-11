/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "lambda.hh"

#include <string>
#include <stdexcept>

#include "awsv4_sig.hh"

using namespace std;
using InvocationType = LambdaInvocationRequest::InvocationType;
using LogType = LambdaInvocationRequest::LogType;

string to_string( const InvocationType & invocation_type )
{
  switch ( invocation_type ) {
  case InvocationType::EVENT: return "Event";
  case InvocationType::REQUEST_RESPONSE: return "RequestResponse";
  case InvocationType::DRY_RUN: return "DryRun";

  default: throw runtime_error( "invalid invocation type" );
  }
}

string to_string( const LogType & log_type )
{
  switch( log_type ) {
  case LogType::NONE: return "None";
  case LogType::TAIL: return "Tail";

  default: throw runtime_error( "invalid log type" );
  }
}

std::string LambdaInvocationRequest::endpoint( const std::string & region )
{
  return "lambda." + region + ".amazonaws.com";
}

LambdaInvocationRequest::LambdaInvocationRequest( const AWSCredentials & credentials,
                                                  const string & region,
                                                  const string & function_name,
                                                  const string & payload,
                                                  const InvocationType invocation_type,
                                                  const LogType & log_type,
                                                  const string & context )
  : AWSRequest( credentials, region, {}, payload )
{
  const string path = "/2015-03-31/functions/" + function_name + "/invocations";
  first_line_ = "POST " + path + " HTTP/1.1";

  headers_[ "host" ] = endpoint( region );
  headers_[ "content-length" ] = to_string( payload.length() );
  headers_[ "x-amz-invocation-type" ] = to_string( invocation_type );
  headers_[ "x-amz-log-type" ] = to_string( log_type );
  headers_[ "x-amz-client-context" ] = context;

  AWSv4Sig::sign_request( "POST\n" + path,
                          credentials_.secret_key(), credentials_.access_key(),
                          region_, "lambda", request_date_, payload, headers_ );
}
