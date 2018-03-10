/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "engine_lambda.hh"

#include <stdexcept>
#include <cmath>

#include "execution_response.hh"
#include "thunk/ggutils.hh"
#include "net/http_response.hh"
#include "util/optional.hh"
#include "util/system_runner.hh"
#include "util/units.hh"

using namespace std;
using namespace gg::thunk;

HTTPRequest AWSLambdaExecutionEngine::generate_request( const Thunk & thunk )
{
  string function_name;

  if ( getenv( "GG_GENERIC_FUNCTION" ) == nullptr ) {
    function_name = "gg-" + thunk.executable_hash();
  }
  else {
    function_name = "gg-function-generic";
  }

  return LambdaInvocationRequest(
    credentials_, region_, function_name,
    Thunk::execution_payload( thunk ),
    LambdaInvocationRequest::InvocationType::REQUEST_RESPONSE,
    LambdaInvocationRequest::LogType::NONE
  ).to_http_request();
}

void AWSLambdaExecutionEngine::force_thunk( const Thunk & thunk,
                                            ExecutionLoop & exec_loop )
{
  HTTPRequest request = generate_request( thunk );

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

  SecureSocket lambda_socket = ssl_context_.new_secure_socket( move( sock ) );

  uint64_t exec_id = exec_loop.add_connection(
    thunk.hash(),
    [this] ( const uint64_t id, const string & thunk_hash,
             const HTTPResponse & http_response )
    {
      running_jobs_--;

      if ( http_response.status_code() != "200" ) {
        if ( http_response.status_code() == "429" or
             ( http_response.status_code() == "500" and
               http_response.has_header( "x-amzn-ErrorType" ) and
               http_response.get_header_value( "x-amzn-ErrorType" ) == "ServiceException" ) ) {
          return failure_callback_( thunk_hash, JobStatus::RateLimit );
        }
        else {
          return failure_callback_( thunk_hash, JobStatus::InvocationFailure );
        }
      }

      ExecutionResponse response = ExecutionResponse::parse_message( http_response.body() );

      /* print the output, if there's any */
      if ( response.output.length() ) {
        cerr << response.output << endl;
      }

      switch ( response.status ) {
      case JobStatus::Success:
        if ( response.thunk_hash != thunk_hash ) {
          cerr << http_response.str() << endl;
          throw runtime_error( "expected output for " +
                               thunk_hash + ", got output for " +
                               response.thunk_hash );
        }

        gg::cache::insert( response.thunk_hash, response.output_hash );
        success_callback_( response.thunk_hash, response.output_hash,
                           compute_cost( start_times_.at( id ) ) );

        start_times_.erase( id );
        break;

      default: /* in case of any other failure */
        failure_callback_( thunk_hash, response.status );
      }
    },
    [this] ( const uint64_t id, const string & thunk_hash )
    {
      start_times_.erase( id );
      failure_callback_( thunk_hash, JobStatus::SocketFailure );
    },
    lambda_socket, request
  );

  start_times_.insert( { exec_id, chrono::steady_clock::now() } );

  running_jobs_++;
}

size_t AWSLambdaExecutionEngine::job_count() const
{
  return running_jobs_;
}

bool AWSLambdaExecutionEngine::can_execute( const gg::thunk::Thunk & thunk ) const
{
  return thunk.infiles_size() < 200_MiB;
}

float AWSLambdaExecutionEngine::compute_cost( const chrono::steady_clock::time_point & begin,
                                              const chrono::steady_clock::time_point & end )
{
  const static float unit_cost = 2.501e-6;

  chrono::duration<float, std::milli> total_ms = end - begin;
  float total = ceil( total_ms.count() / 100 );

  return unit_cost * total;
}
