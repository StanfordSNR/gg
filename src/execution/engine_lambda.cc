/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "engine_lambda.hh"

#include <stdexcept>
#include <cmath>

#include "response.hh"
#include "thunk/ggutils.hh"
#include "net/http_response.hh"
#include "net/nb_secure_socket.hh"
#include "util/base64.hh"
#include "util/optional.hh"
#include "util/system_runner.hh"
#include "util/units.hh"

using namespace std;
using namespace gg::thunk;

HTTPRequest AWSLambdaExecutionEngine::generate_request( const vector<Thunk> & thunk )
{
  string function_name;

  if ( getenv( "GG_SPECIALIZED_FUNCTION" ) == nullptr ) {
    function_name = "gg-lambda-function";
  }
  else {
    function_name = "gg-" + thunk[0].executable_hash();
  }

  return LambdaInvocationRequest(
    credentials_, region_, function_name,
    Thunk::execution_payload( thunk ),
    LambdaInvocationRequest::InvocationType::REQUEST_RESPONSE,
    LambdaInvocationRequest::LogType::NONE
  ).to_http_request();
}

void AWSLambdaExecutionEngine::force_thunk( const vector<Thunk> & thunk,
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

  string comb_hash = "";
  for ( const Thunk & t : thunk ) {
    comb_hash += t.hash();
  }

  uint64_t connection_id = exec_loop.make_http_request<SECURE>( comb_hash,
    address_, request,
    [this] ( const uint64_t id, const string & thunk_hash,
             const HTTPResponse & http_response ) -> bool
    {
      running_jobs_--;

      if ( http_response.status_code() != "200" ) {
        if ( http_response.status_code() == "429" or
             ( http_response.status_code() == "500" and
               http_response.has_header( "x-amzn-ErrorType" ) and
               http_response.get_header_value( "x-amzn-ErrorType" ) == "ServiceException" ) ) {
          failure_callback_( thunk_hash, JobStatus::RateLimit );
          return false;
        }
        else {
          failure_callback_( thunk_hash, JobStatus::InvocationFailure );
          return false;
        }
      }

      ExecutionResponse response = ExecutionResponse::parse_message( http_response.body() );

      /* print the output, if there's any */
      if ( response.stdout.length() ) {
        cerr << response.stdout << endl;
      }

      switch ( response.status ) {
      case JobStatus::Success:
        {
          string check_comb_hash = "";
          for ( const string & hash : response.thunk_hash ) {
            check_comb_hash += hash;
          }

          if ( check_comb_hash != thunk_hash ) {
            cerr << http_response.str() << endl;
            throw runtime_error( "expected output for " +
                                 thunk_hash + ", got output for " +
                                 check_comb_hash );
          }

          uint32_t th_iter = 0;
          for ( auto resp : response.outputs ) {
            string next_hash = response.thunk_hash.at( th_iter );

            for ( const auto & output : resp ) {
              gg::cache::insert( gg::hash::for_output( next_hash, output.tag ), output.hash );

              if ( output.data.length() ) {
                roost::atomic_create( base64::decode( output.data ),
                                      gg::paths::blob_path( output.hash ) );
              }
            }

            gg::cache::insert( next_hash, response.outputs.at( th_iter ).at( 0 ).hash );

            th_iter++;
          }
          success_callback_( response.thunk_hash.at( th_iter - 1 ), 
                             response.outputs.at( th_iter - 1 ).at( 0 ).hash,
                             compute_cost( start_times_.at( id ) ) );

          start_times_.erase( id );
          break;
        }

      default: /* in case of any other failure */
        failure_callback_( thunk_hash, response.status );
      }

      return false;
    },
    [this] ( const uint64_t id, const string & thunk_hash )
    {
      start_times_.erase( id );
      failure_callback_( thunk_hash, JobStatus::SocketFailure );
    }
  );

  start_times_.insert( { connection_id, chrono::steady_clock::now() } );

  running_jobs_++;
}

size_t AWSLambdaExecutionEngine::job_count() const
{
  return running_jobs_;
}

bool AWSLambdaExecutionEngine::can_execute( const vector<gg::thunk::Thunk> & thunk ) const
{
  for ( const Thunk & t : thunk ) {
    if ( t.infiles_size() >= 200_MiB ) {
      return false;
    }
  }
  return true;
}

float AWSLambdaExecutionEngine::compute_cost( const chrono::steady_clock::time_point & begin,
                                              const chrono::steady_clock::time_point & end )
{
  const static float unit_cost = 2.501e-6;

  chrono::duration<float, std::milli> total_ms = end - begin;
  float total = ceil( total_ms.count() / 100 );

  return unit_cost * total;
}
