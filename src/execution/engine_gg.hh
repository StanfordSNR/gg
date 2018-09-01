/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef ENGINE_GG_HH
#define ENGINE_GG_HH

#include "engine.hh"
#include "net/http_request.hh"
#include "thunk/thunk.hh"

class GGExecutionEngine : public ExecutionEngine
{
private:
  Address address_;

  size_t running_jobs_ { 0 };

  HTTPRequest generate_request( const gg::thunk::Thunk & thunk );

public:
  GGExecutionEngine( const size_t max_jobs, const Address & address )
    : ExecutionEngine( max_jobs ), address_( address )
  {}

  void force_thunk( const gg::thunk::Thunk & thunk,
                    ExecutionLoop & exec_loop ) override;
  size_t job_count() const override;

  bool is_remote() const { return true; }
  std::string label() const override { return "remote"; }
  bool can_execute( const gg::thunk::Thunk & ) const { return true; }
};

#endif /* ENGINE_GG_HH */
