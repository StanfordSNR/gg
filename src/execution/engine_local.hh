/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef ENGINE_LOCAL_HH
#define ENGINE_LOCAL_HH

#include "engine.hh"

class LocalExecutionEngine : public ExecutionEngine
{
private:
  size_t running_jobs_ { 0 };

public:
  using ExecutionEngine::ExecutionEngine;

  void force_thunk( const std::string & hash, const gg::thunk::Thunk & thunk, ExecutionLoop & exec_loop ) override;
  size_t job_count() const override;

  bool is_remote() const override { return false; }
  std::string label() const override { return "local"; }
  bool can_execute( const gg::thunk::Thunk & ) const override { return true; }
};

#endif /* ENGINE_LOCAL_HH */
