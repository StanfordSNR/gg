/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef ENGINE_GG_HH
#define ENGINE_GG_HH

#include "engine.hh"
#include "thunk.hh"
#include "http_request.hh"

class GGExecutionEngine : public ExecutionEngine
{
private:
  Address address_;

  size_t running_jobs_ { 0 };

  HTTPRequest generate_request( const gg::thunk::Thunk & thunk,
                                const std::string & thunk_hash,
                                const bool timelog );

public:
  GGExecutionEngine( const std::string & address, const uint16_t port,
                     SuccessCallbackFunc success_callback,
                     FailureCallbackFunc failure_callback )
    : ExecutionEngine( success_callback, failure_callback ),
      address_( address, port )
  {}

  void force_thunk( const std::string & hash, const gg::thunk::Thunk & thunk, ExecutionLoop & exec_loop ) override;
  size_t job_count() const override;

  bool is_remote() const { return true; }
  std::string label() const override { return "gg-remote"; }
  bool can_execute( const gg::thunk::Thunk & ) const { return true; }
};

#endif /* ENGINE_GG_HH */
