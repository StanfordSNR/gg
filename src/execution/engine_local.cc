/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "engine_local.hh"

#include <stdexcept>

#include "ggpaths.hh"
#include "optional.hh"
#include "system_runner.hh"

using namespace std;
using namespace gg::thunk;

void LocalExecutionEngine::callback( const string & hash )
{
  Optional<gg::cache::ReductionResult> result = gg::cache::check( hash );

  if ( not result.initialized() or result->order != 0 ) {
    throw runtime_error( "could not find the reduction entry" );
  }
}

void LocalExecutionEngine::force_thunk( const string & hash,
                                        const Thunk & /* thunk */ )
{
  exec_loop_.add_child_process( hash,
    &LocalExecutionEngine::callback,
    [hash]()
    {
      vector<string> command { "gg-execute", hash };
      return ezexec( command[ 0 ], command, {}, true, true );
    }
  );
}
