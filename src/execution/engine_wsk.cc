/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "engine_wsk.hh"

#include <stdexcept>
#include <cmath>
#include <unordered_map>

#include "ggpaths.hh"
#include "optional.hh"
#include "system_runner.hh"
#include "http_response.hh"
#include "remote_response.hh"
#include "units.hh"
#include "digest.hh"

using namespace std;
using namespace gg::thunk;

pair<string, string> OpenWhiskExecutionEngine::get_host_and_path( const string & url )
{
  string result { url };

  result = result.substr( 8 ); /* remove https:// */
  auto first_slash = result.find( '/' );

  if ( first_slash == string::npos ) {
    return { result, "/" };
  }

  return { result.substr( 0, first_slash ), result.substr( first_slash ) };
}

HTTPRequest OpenWhiskExecutionEngine::generate_request( const Thunk & thunk,
                                                        const string & thunk_hash )
{
  unordered_map<string, string> extra = {
    { "kkv_host", gg::remote::kkv_host() },
    { "kkv_username", gg::remote::kkv_username() },
    { "kkv_password", gg::remote::kkv_password() },
    { "user", gg::remote::trpz_username() },
    { "pass", gg::remote::trpz_password() },
  };

  string payload = thunk.execution_payload( thunk_hash, false, extra );

  HTTPRequest request;
  request.set_first_line( "POST " + path_ + "?blocking=true&result=true HTTP/1.0" );
  request.add_header( HTTPHeader{ "Host", hostname_ } );
  request.add_header( HTTPHeader{ "Content-Type", "application/json" } );
  request.add_header( HTTPHeader{ "Content-Length", to_string( payload.size() ) } );
  request.add_header( HTTPHeader{ "Accept", "application/json" } );
  request.add_header( HTTPHeader{ "Authorization", "Basic " + base64::encode( auth_ ) } );
  request.done_with_headers();

  request.read_in_body( payload );
  assert( request.state() == COMPLETE );

  return request;
}

void OpenWhiskExecutionEngine::force_thunk( const string & hash,
                                            const Thunk & thunk )
{
  HTTPRequest request = generate_request( thunk, hash );

  TCPSocket sock;
  sock.set_blocking( false );
  try {
    sock.connect( address_ );
    throw runtime_error( "nonblocking connect unexpectedly succeeded immediately" );
  } catch ( const unix_error & e ) {
    if ( e.error_code() == EINPROGRESS ) {
      /* do nothing */
    } else {
      throw;
    }
  }

  SecureSocket socket = ssl_context_.new_secure_socket( move( sock ) );

  exec_loop_.add_connection(
    hash,
    [this] ( const string & thunk_hash, const HTTPResponse & http_response )
    {
      running_jobs_--;

      if ( http_response.status_code() != "200" ) {
        throw runtime_error( "HTTP failure: " + http_response.status_code() );
      }

      RemoteResponse response = RemoteResponse::parse_message( http_response.body() );

      if ( response.type != RemoteResponse::Type::SUCCESS ) {
        throw runtime_error( "execution failed." );
      }

      if ( response.thunk_hash != thunk_hash ) {
        cerr << http_response.str() << endl;
        throw runtime_error( "expected output for " + thunk_hash + ", got output for " + response.thunk_hash );
      }

      gg::cache::insert( response.thunk_hash, response.output_hash );
      callback_( response.thunk_hash, response.output_hash,
                 compute_cost( start_times_.at( thunk_hash ) ) );

      start_times_.erase( thunk_hash );
    },
    socket, request
  );

  start_times_.insert( { hash, chrono::steady_clock::now() } );

  running_jobs_++;
}

size_t OpenWhiskExecutionEngine::job_count() const
{
  return running_jobs_;
}

bool OpenWhiskExecutionEngine::can_execute( const gg::thunk::Thunk & thunk ) const
{
  return thunk.infiles_size() < 200_MiB;
}
