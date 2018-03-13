/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "engine_local.hh"

#include <stdexcept>

#include "thunk/ggutils.hh"
#include "util/optional.hh"
#include "util/system_runner.hh"

using namespace std;
using namespace gg::thunk;

void LocalExecutionEngine::force_thunk( const Thunk & thunk,
                                        ExecutionLoop & exec_loop )
{
  exec_loop.add_child_process( thunk.hash(),
    [this] ( const uint64_t, const string & hash )
    {
      running_jobs_--; /* XXX not thread-safe */

      Optional<gg::cache::ReductionResult> result = gg::cache::check( hash );

      if ( not result.initialized() ) {
        throw runtime_error( "could not find the reduction entry" );
      }

      success_callback_( hash, result->hash, 0 );
    },
    [this] ( const uint64_t, const string & thunk_hash )
    {
      failure_callback_( thunk_hash, JobStatus::ChildProcessFailure );
    },
    [&thunk]()
    {
      vector<string> command { "gg-execute", thunk.hash() };
      return ezexec( command[ 0 ], command, {}, true, true );
    }
  );

  running_jobs_++;
}

size_t LocalExecutionEngine::job_count() const
{
  return running_jobs_;
}
