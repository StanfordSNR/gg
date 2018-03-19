/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "engine_gg.hh"

#include <stdexcept>

#include "response.hh"
#include "net/http_response.hh"
#include "thunk/ggutils.hh"
#include "util/optional.hh"
#include "util/system_runner.hh"
#include "util/units.hh"

using namespace std;
using namespace gg::thunk;

HTTPRequest GGExecutionEngine::generate_request( const Thunk & thunk )
{
  string payload = Thunk::execution_payload( thunk );
  HTTPRequest request;
  request.set_first_line( "POST /cgi-bin/gg/execute.cgi HTTP/1.1" );
  request.add_header( HTTPHeader{ "Content-Length", to_string( payload.size() ) } );
  request.add_header( HTTPHeader{ "Host", "gg-run-server" } );
  request.done_with_headers();

  request.read_in_body( payload );
  assert( request.state() == COMPLETE );

  return request;
}

void GGExecutionEngine::force_thunk( const Thunk & thunk,
                                     ExecutionLoop & exec_loop )
{
  HTTPRequest request = generate_request( thunk );

  TCPSocket socket;
  socket.set_blocking( false );

  try {
    socket.connect( address_ );
    throw runtime_error( "nonblocking connect unexpectedly succeeded immediately" );
  } catch ( const unix_error & e ) {
    if ( e.error_code() == EINPROGRESS ) {
      /* do nothing */
    } else {
      throw;
    }
  }

  exec_loop.add_connection(
    thunk.hash(),
    [this] ( const uint64_t, const string & thunk_hash,
             const HTTPResponse & http_response )
    {
      running_jobs_--;

      if ( http_response.status_code() != "200" ) {
        throw runtime_error( "HTTP failure: " + http_response.status_code() );
      }

      ExecutionResponse response = ExecutionResponse::parse_message( http_response.body() );

      if ( response.status != JobStatus::Success ) {
        throw runtime_error( "execution failed." );
      }

      if ( response.thunk_hash != thunk_hash ) {
        cerr << http_response.str() << endl;
        throw runtime_error( "expected output for " + thunk_hash + ", got output for " + response.thunk_hash );
      }

      for ( const auto & output : response.outputs ) {
        gg::cache::insert( gg::hash::for_output( response.thunk_hash, output.tag ), output.hash );

        if ( output.data.length() ) {
          atomic_create( output.data, gg::paths::blob_path( output.hash ) );
        }
      }

      gg::cache::insert( response.thunk_hash, response.outputs.at( 0 ).hash );
      success_callback_( response.thunk_hash, response.outputs.at( 0 ).hash, 0 );
    },
    [this] ( const uint64_t, const string & thunk_hash )
    {
      failure_callback_( thunk_hash, JobStatus::SocketFailure );
    },
    socket, request
  );

  running_jobs_++;
}

size_t GGExecutionEngine::job_count() const
{
  return running_jobs_;
}
