/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef ENGINE_MEOW_HH
#define ENGINE_MEOW_HH

#include "engine.hh"
#include "net/address.hh"
#include "net/aws.hh"
#include "net/lambda.hh"

class MeowExecutionEngine : public ExecutionEngine
{
public:
  MeowExecutionEngine( const AWSCredentials & credentials,
                       const std::string & region,
                       const Address & listen_addr,
                       ExecutionLoop & loop,
                       SuccessCallbackFunc success_callback,
                       FailureCallbackFunc failure_callback );

  void force_thunk( const gg::thunk::Thunk & thunk,
                    ExecutionLoop & exec_loop ) override;

  bool is_remote() const { return true; }
  bool can_execute( const gg::thunk::Thunk & thunk ) const override;
  size_t job_count() const override;
  std::string label() const override { return "meow"; }
};

#endif /* ENGINE_MU_HH */
