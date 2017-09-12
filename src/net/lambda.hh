/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef LAMBDA_HH
#define LAMBDA_HH

#include <string>

#include "aws.hh"

class LambdaInvocationRequest : public AWSRequest
{
public:
  enum class InvocationType { EVENT, REQUEST_RESPONSE, DRY_RUN };
  enum class LogType { NONE, TAIL };

  static std::string endpoint( const std::string & region );

  LambdaInvocationRequest( const AWSCredentials & credentials,
                           const std::string & region,
                           const std::string & function_name,
                           const std::string & payload,
                           const InvocationType invocation_type = InvocationType::REQUEST_RESPONSE,
                           const LogType & log_type = LogType::NONE,
                           const std::string & context = "e30=" /* base64( "{}" ) */ );
};

#endif /* LAMBDA_HH */
