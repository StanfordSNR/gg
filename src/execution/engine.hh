/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef ENGINE_HH
#define ENGINE_HH

#include <string>
#include <functional>

#include "loop.hh"
#include "thunk.hh"

class ExecutionEngine
{
public:
  typedef std::function<void( const std::string &, const std::string & )> CallbackFunc;

protected:
  ExecutionLoop & exec_loop_; /* XXX */
  std::function<void( const std::string &, const std::string & )> callback_;

public:
  ExecutionEngine( ExecutionLoop & loop, CallbackFunc callback )
    : exec_loop_( loop ), callback_( callback )
  {}

  virtual void force_thunk( const std::string & hash, const gg::thunk::Thunk & thunk ) = 0;
  virtual size_t job_count() const = 0;

  virtual ~ExecutionEngine() {}
};

#endif /* ENGINE_HH */
