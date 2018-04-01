/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef ENGINE_HH
#define ENGINE_HH

#include <string>
#include <functional>
#include <vector>

#include "loop.hh"
#include "response.hh"
#include "thunk/thunk.hh"

class ExecutionEngine
{
public:
  /* success_callback( vector<source_hash, target_hash> estimated_cost ) */
  typedef std::function<void( const std::string &,
                              const std::string &,
                              const float )>
          SuccessCallbackFunc;

  /* failure_callback( source_hash, failure_reason ) */
  typedef std::function<void( const std::string &,
                              const JobStatus )>
          FailureCallbackFunc;

protected:
  SuccessCallbackFunc success_callback_ {};
  FailureCallbackFunc failure_callback_ {};

public:
  void set_success_callback( SuccessCallbackFunc func ) { success_callback_ = func; }
  void set_failure_callback( FailureCallbackFunc func ) { failure_callback_ = func; }

  virtual void init( ExecutionLoop & ) {}
  virtual void force_thunk( const std::vector<gg::thunk::Thunk> & thunk,
                            ExecutionLoop & exec_loop ) = 0;
  virtual bool is_remote() const = 0;
  virtual bool can_execute( const std::vector<gg::thunk::Thunk> & thunk ) const = 0;
  virtual size_t job_count() const = 0;
  virtual std::string label() const = 0;

  virtual ~ExecutionEngine() {}
};

#endif /* ENGINE_HH */
