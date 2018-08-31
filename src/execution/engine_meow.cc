/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "engine_meow.hh"

#include <iostream>

#include "protobufs/gg.pb.h"
#include "protobufs/meow.pb.h"
#include "protobufs/util.hh"
#include "thunk/ggutils.hh"
#include "execution/meow/message.hh"
#include "execution/meow/util.hh"
#include "util/base64.hh"
#include "util/iterator.hh"
#include "util/units.hh"

using namespace std;
using namespace gg;
using namespace meow;
using namespace gg::thunk;
using namespace PollerShortNames;

HTTPRequest MeowExecutionEngine::generate_request()
{
  static const bool timelog = ( getenv( "GG_TIMELOG" ) != nullptr );

  string function_name = "gg-meow-function";

  gg::protobuf::meow::InvocationRequest request;
  request.set_coordinator( listen_addr_.str() );
  request.set_storage_backend( gg::remote::storage_backend_uri() );
  request.set_timelog( timelog );

  return LambdaInvocationRequest(
    credentials_, region_, function_name,
    protoutil::to_json( request ),
    LambdaInvocationRequest::InvocationType::EVENT,
    LambdaInvocationRequest::LogType::NONE
  ).to_http_request();
}

MeowExecutionEngine::MeowExecutionEngine( const size_t max_jobs,
                                          const AWSCredentials & credentials,
                                          const std::string & region,
                                          const Address & listen_addr )
  : ExecutionEngine( max_jobs ), credentials_( credentials ), region_( region ),
    aws_addr_( LambdaInvocationRequest::endpoint( region_ ), "https" ),
    listen_addr_( listen_addr ), listen_socket_()
{}

void MeowExecutionEngine::init( ExecutionLoop & exec_loop )
{
  exec_loop.make_listener( { "0.0.0.0", listen_addr_.port() },
    [this] ( ExecutionLoop & loop, TCPSocket && socket ) {
      cerr << "[meow] Incoming connection: "
           << socket.peer_address().str() << endl;

      auto message_parser = make_shared<meow::MessageParser>();

      auto connection = loop.add_connection<TCPSocket>( move( socket ),
        [message_parser, id=current_id_, this] ( shared_ptr<TCPConnection>, string && data ) {
          message_parser->parse( data );

          while ( not message_parser->empty() ) {
            /* we got a message! */
            const Message & message = message_parser->front();

            switch ( message.opcode() ) {
            case Message::OpCode::Hey:
              // cerr << "[meow:worker@" << id << ":hey] " << message.payload() << endl;
              break;

            case Message::OpCode::Put:
            {
              const string hash = handle_put_message( message );
              // cerr << "[meow:worker@" << id << ":put] " << hash << endl;
              break;
            }

            case Message::OpCode::Executed:
            {
              protobuf::ResponseItem execution_response;
              protoutil::from_string( message.payload(), execution_response );

              const string & thunk_hash = execution_response.thunk_hash();
              // cerr << "[meow:worker@" << id << ":executed] " << thunk_hash << endl;

              for ( const auto & output : execution_response.outputs() ) {
                gg::cache::insert( gg::hash::for_output( thunk_hash, output.tag() ), output.hash() );
                // XXX gg::remote::set_available( output.hash() );

                if ( output.data().length() ) {
                  roost::atomic_create( base64::decode( output.data() ),
                                        gg::paths::blob( output.hash() ) );
                }
              }

              gg::cache::insert( thunk_hash, execution_response.outputs( 0 ).hash() );
              lambdas_.at( id ).state = Lambda::State::Idle;
              free_lambdas_.insert( id );
              running_jobs_--;

              vector<ThunkOutput> thunk_outputs;
              for ( auto & output : execution_response.outputs() ) {
                thunk_outputs.emplace_back( move( output.hash() ), move( output.tag() ) );
              }

              success_callback_( thunk_hash, move( thunk_outputs ), 0 );

              break;
            }

            default:
              throw runtime_error( "unexpected opcode" );
            }

            message_parser->pop();
          }

          return true;
        },
        [] () {
          throw runtime_error( "error occurred" );
        },
        [this] () {
          throw runtime_error( "a lambda died" );
        }
      );

      lambdas_.emplace( piecewise_construct,
                        forward_as_tuple( current_id_ ),
                        forward_as_tuple( current_id_, move( connection ) ) );

      free_lambdas_.emplace( current_id_ );

      if ( not thunks_queue_.empty() ) {
        prepare_lambda( lambdas_.at( current_id_ ), thunks_queue_.front() );
        thunks_queue_.pop();
      }

      current_id_++;
      return true;
    }
  );

  cerr << "[meow] Listening for incoming connections on " << listen_addr_.str() << endl;
}

void MeowExecutionEngine::prepare_lambda( Lambda & lambda, const Thunk & thunk )
{
  /** (1) send all the dependencies **/
  unordered_set<string> lambda_objects;
  for ( const auto & item : join_containers( thunk.values(), thunk.executables() ) ) {
    if ( not lambda.objects.count( item.first ) /* XXX and
         not gg::remote::is_available( item.first ) */ ) {
      lambda.connection->enqueue_write( meow::create_put_message( item.first ).str() );
    }

    lambda_objects.insert( item.first );
  }

  lambda.objects = move( lambda_objects );

  /** (2) send the request for thunk execution */
  lambda.connection->enqueue_write( meow::create_execute_message( thunk ).str() );

  /** (3) update Lambda's state **/
  lambda.state = Lambda::State::Busy;
  free_lambdas_.erase( lambda.id );
  lambda.executing_thunk.reset( thunk );

  /** (4) ??? **/

  /** (5) PROFIT **/
}

uint64_t MeowExecutionEngine::pick_lambda( const Thunk & thunk,
                                           const SelectionStrategy s )
{
  if ( free_lambdas_.size() == 0 ) {
    throw runtime_error( "No free lambdas to pick from." );
  }

  switch ( s ) {
  case SelectionStrategy::First:
    return *free_lambdas_.begin();

  case SelectionStrategy::MostObjects:
  {
    size_t max_common_size = 0;
    size_t selected_lambda = numeric_limits<size_t>::max();

    unordered_set<string> thunk_objects;
    for ( const auto & item : join_containers( thunk.values(), thunk.executables() ) ) {
      thunk_objects.insert( item.first );
    }

    for ( const auto & free_lambda : free_lambdas_ ) {
      const Lambda & lambda = lambdas_.at( free_lambda );
      size_t common_size = 0;

      for ( const string & obj : thunk_objects ) {
        if ( lambda.objects.count( obj ) ) {
          common_size += gg::hash::size( obj );
        }
      }

      if ( common_size > max_common_size ) {
        selected_lambda = free_lambda;
        max_common_size = common_size;
      }
    }

    if ( selected_lambda != numeric_limits<size_t>::max() ) {
      cerr << "picked a lambda with " << max_common_size << " bytes in common" << endl;
      return selected_lambda;
    }
    else {
      return *free_lambdas_.begin();
    }
  }

  case SelectionStrategy::LargestObject:
  {
    /* what's the largest object in this thunk? */
    string largest_hash;
    uint32_t largest_size = 0;

    for ( const auto & item : join_containers( thunk.values(), thunk.executables() ) ) {
      if ( gg::hash::size( item.first ) > largest_size ) {
        largest_size = gg::hash::size( item.first );
        largest_hash = item.first;
      }
    }

    if ( largest_hash.length() ) {
      for ( const auto & free_lambda : free_lambdas_ ) {
        if ( lambdas_.at( free_lambda ).objects.count( largest_hash ) ) {
          return free_lambda;
        }
      }
    }

    /* if we couldn't find anything */
    return *free_lambdas_.begin();
  }

  default:
    throw runtime_error( "invalid selection strategy" );
  }

  return *free_lambdas_.begin();
}

void MeowExecutionEngine::force_thunk( const Thunk & thunk, ExecutionLoop & loop )
{
  cerr << "[meow] force " << thunk.hash() << endl;
  running_jobs_++;
  /* do we have a free Lambda for this? */
  if ( free_lambdas_.size() > 0 ) {
    /* execute the job on that Lambda */
    const uint64_t picked_lambda = pick_lambda( thunk, SelectionStrategy::MostObjects );
    return prepare_lambda( lambdas_.at( picked_lambda ), thunk );
  }

  /* there are no free Lambdas, let's launch one */
  thunks_queue_.push( thunk );

  loop.make_http_request<SSLConnection>( "start-worker", aws_addr_,
    generate_request(),
    [] ( const uint64_t, const string &, const HTTPResponse & ) {
      cerr << "[meow] invoked a lambda" << endl;
    },
    [] ( const uint64_t, const string & ) {
      cerr << "invocation request failed" << endl;
    }
  );
}

bool MeowExecutionEngine::can_execute( const gg::thunk::Thunk & thunk ) const
{
  return thunk.infiles_size() < 200_MiB;
}

size_t MeowExecutionEngine::job_count() const
{
  return running_jobs_;
}
