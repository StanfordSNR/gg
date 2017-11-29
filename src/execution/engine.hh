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
  typedef std::function<void( const std::string &, const std::string &, const float )> CallbackFunc;

protected:
  CallbackFunc callback_;

public:
  ExecutionEngine( CallbackFunc callback )
    : callback_( callback )
  {}

  virtual void force_thunk( const std::string & hash, const gg::thunk::Thunk & thunk, ExecutionLoop & exec_loop ) = 0;
  virtual size_t job_count() const = 0;

  virtual bool is_remote() const = 0;
  virtual std::string label() const = 0;
  virtual bool can_execute( const gg::thunk::Thunk & thunk ) const = 0;

  virtual ~ExecutionEngine() {}
};

#endif /* ENGINE_HH */
