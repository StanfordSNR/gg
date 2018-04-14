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
#include "util/units.hh"

using namespace std;
using namespace gg;
using namespace meow;
using namespace gg::thunk;
using namespace PollerShortNames;

HTTPRequest MeowExecutionEngine::generate_request()
{
  string function_name = "gg-meow-function";

  gg::protobuf::meow::InvocationRequest request;
  request.set_coordinator( listen_addr_.str() );

  return LambdaInvocationRequest(
    credentials_, region_, function_name,
    protoutil::to_json( request ),
    LambdaInvocationRequest::InvocationType::EVENT,
    LambdaInvocationRequest::LogType::NONE
  ).to_http_request();
}

MeowExecutionEngine::MeowExecutionEngine( const AWSCredentials & credentials,
                                          const std::string & region,
                                          const Address & listen_addr )
  : credentials_( credentials ), region_( region ),
    aws_addr_( LambdaInvocationRequest::endpoint( region_ ), "https" ),
    listen_addr_( listen_addr ), listen_socket_()
{}

void MeowExecutionEngine::init( ExecutionLoop & exec_loop )
{
  exec_loop.make_listener( listen_addr_,
    [this] ( ExecutionLoop & loop, shared_ptr<TCPConnection> & connection ) {
      cerr << "[meow] Incoming connection: "
           << connection->socket().peer_address().str() << endl;

      auto message_parser = make_shared<meow::MessageParser>();

      loop.add_connection( connection,
        [message_parser, this] ( const string & data ) {
          message_parser->parse( data );

          while ( not message_parser->empty() ) {
            /* we got a message! */
            cerr << "[meow] msg,opcode="
                 << static_cast<uint32_t>( message_parser->front().opcode() )
                 << endl;

            const Message & message = message_parser->front();

            switch ( message.opcode() ) {
            case Message::OpCode::Put:
              handle_put_message( message );
              break;

            case Message::OpCode::Executed:
            {
              protobuf::ResponseItem execution_response;
              protoutil::from_json( message.payload(), execution_response );

              const string & thunk_hash = execution_response.thunk_hash();

              for ( const auto & output : execution_response.outputs() ) {
                gg::cache::insert( gg::hash::for_output( thunk_hash, output.tag() ), output.hash() );

                if ( output.data().length() ) {
                  roost::atomic_create( base64::decode( output.data() ),
                                        gg::paths::blob_path( output.hash() ) );
                }
              }

              gg::cache::insert( thunk_hash, execution_response.outputs( 0 ).hash() );
              success_callback_( thunk_hash, execution_response.outputs( 0 ).hash(), 0 );

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
          cerr << "Error occured!" << endl;
        },
        [] () {
          cerr << "Connection closed." << endl;
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
  /* XXX should only send the stuff that are not there */
  for ( const auto & item : thunk.values() ) {
    lambda.connection->enqueue_write( meow::create_put_message( item.first ).to_string() );
  }

  for ( const auto & item : thunk.executables() ) {
    lambda.connection->enqueue_write( meow::create_put_message( item.first ).to_string() );
  }

  /** (2) send the request for thunk execution */
  lambda.connection->enqueue_write( meow::create_execute_message( thunk ).to_string() );

  /** (3) update Lambda's state **/
  lambda.state = Lambda::State::Busy;
  free_lambdas_.erase( lambda.id );

  /** (4) ??? **/

  /** (5) PROFIT **/
}

void MeowExecutionEngine::force_thunk( const vector<Thunk> & thunks, ExecutionLoop & )
{
  const Thunk & thunk = thunks[0];

  /* do we have a free Lambda for this? */
  if ( free_lambdas_.size() > 0 ) {
    /* execute the job on that Lambda */
    return prepare_lambda( lambdas_.at( *free_lambdas_.begin() ), thunk );
  }

  /* there are no free Lambdas, let's launch one */
  thunks_queue_.push( thunk );

  /* loop.make_http_request<SSLConnection>( "start-worker", aws_addr_,
    generate_request(),
    [] ( const uint64_t, const string &, const HTTPResponse & response ) {
      cerr << response.str() << endl;
    },
    [] ( const uint64_t, const string & ) {
      cerr << "invocation request failed" << endl;
    }
  );*/
}

bool MeowExecutionEngine::can_execute( const vector<gg::thunk::Thunk> & thunk ) const
{
  for ( const Thunk & t : thunk ) {
    if ( t.infiles_size() >= 200_MiB ) {
      return false;
    }
  }
  return true;
}

size_t MeowExecutionEngine::job_count() const
{
  return 0;
}
