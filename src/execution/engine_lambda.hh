/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef ENGINE_LAMBDA_HH
#define ENGINE_LAMBDA_HH

#include "engine.hh"
#include "aws.hh"
#include "lambda.hh"
#include "thunk.hh"
#include "http_request.hh"

class AWSLambdaExecutionEngine : public ExecutionEngine
{
private:
  AWSCredentials credentials_;
  std::string region_;
  SSLContext ssl_context_ {};
  Address address_;

  size_t running_jobs_ { 0 };

  HTTPRequest generate_request( const gg::thunk::Thunk & thunk,
                                const std::string & thunk_hash,
                                const bool timelog );

public:
  AWSLambdaExecutionEngine( const AWSCredentials & credentials,
                            const std::string & region,
                            ExecutionLoop & loop, CallbackFunc callback )
    : ExecutionEngine( loop, callback ), credentials_( credentials ),
      region_( region ),
      address_( LambdaInvocationRequest::endpoint( region_ ), "https" )
  {}

  void force_thunk( const std::string & hash, const gg::thunk::Thunk & thunk ) override;
  size_t job_count() const override;

  bool is_remote() const { return true; }
  std::string label() const override { return "lambda"; }
  bool can_execute( const gg::thunk::Thunk & thunk ) const override;
};

#endif /* ENGINE_LAMBDA_HH */
