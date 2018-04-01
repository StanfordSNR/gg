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

HTTPRequest GGExecutionEngine::generate_request( const vector<Thunk> & thunk )
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

void GGExecutionEngine::force_thunk( const vector<Thunk> & thunk,
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

  string comb_hash = "";
  for ( const Thunk & t : thunk ) {
    comb_hash == t.hash();
  }

  exec_loop.make_http_request<UNSECURE>( comb_hash, address_, request,
    [this] ( const uint64_t, const string & thunk_hash,
             const HTTPResponse & http_response ) -> bool
    {
      running_jobs_--;

      if ( http_response.status_code() != "200" ) {
        throw runtime_error( "HTTP failure: " + http_response.status_code() );
      }

      ExecutionResponse response = ExecutionResponse::parse_message( http_response.body() );

      if ( response.status != JobStatus::Success ) {
        throw runtime_error( "execution failed." );
      }

      string check_comb_hash = "";
      for ( const string & hash : response.thunk_hash ) {
        check_comb_hash += hash;
      }

      if ( check_comb_hash != thunk_hash ) {
        cerr << http_response.str() << endl;
        throw runtime_error( "expected output for " + thunk_hash + ", got output for " + check_comb_hash );
      }

      uint32_t th_iter = 0;
      vector<pair<string, string>> callback_vec;
      for ( auto resp : response.outputs ) {
        string next_hash = response.thunk_hash.at( th_iter );
        for ( const auto & output : resp ) {
          gg::cache::insert( gg::hash::for_output( next_hash, output.tag ), output.hash );

          if ( output.data.length() ) {
            /* XXX: base64::decode( output.data ) ?? */
            roost::atomic_create( output.data, gg::paths::blob_path( output.hash ) );
          }
        }

        gg::cache::insert( next_hash, response.outputs.at( th_iter ).at( 0 ).hash );

        th_iter++;
      }
      success_callback_( response.thunk_hash.at( th_iter - 1 ),
                         response.outputs.at( th_iter - 1 ).at( 0 ).hash,
                         0 );

      return false;
    },
    [this] ( const uint64_t, const string & thunk_hash )
    {
      failure_callback_( thunk_hash, JobStatus::SocketFailure );
    }
  );

  running_jobs_++;
}

size_t GGExecutionEngine::job_count() const
{
  return running_jobs_;
}
