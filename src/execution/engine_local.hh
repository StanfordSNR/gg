/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef ENGINE_LOCAL_HH
#define ENGINE_LOCAL_HH

#include "engine.hh"

class LocalExecutionEngine : public ExecutionEngine
{
public:
  void force_thunk( const std::string & hash, const gg::thunk::Thunk & thunk ) override;

  static void callback( const std::string & hash );
};

#endif /* ENGINE_LOCAL_HH */
