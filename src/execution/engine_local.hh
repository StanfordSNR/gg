/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef ENGINE_LOCAL_HH
#define ENGINE_LOCAL_HH

#include "engine.hh"

class LocalExecutionEngine : public ExecutionEngine
{
private:
  size_t running_jobs_ { 0 };

public:
  LocalExecutionEngine( ExecutionLoop & loop, CallbackFunc callback )
    : ExecutionEngine( loop, callback )
  {}

  void force_thunk( const std::string & hash, const gg::thunk::Thunk & thunk ) override;
  size_t job_count() const override;
};

#endif /* ENGINE_LOCAL_HH */
