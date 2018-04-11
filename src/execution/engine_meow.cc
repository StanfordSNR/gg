/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "engine_meow.hh"

#include <iostream>

#include "util/units.hh"
#include "meow/message.hh"
#include "protobufs/meow.pb.h"
#include "protobufs/util.hh"

using namespace std;
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
        [message_parser] ( const string & data ) {
          message_parser->parse( data );

          if ( not message_parser->empty() ) {
            /* we got a message! */
            cerr << "==== MESSAGE ====" << endl;
            cerr << message_parser->front().payload() << endl;
            cerr << "=================" << endl;
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

      meow::Message message { meow::Message::OpCode::Hey, "Hello, world!" };
      connection->enqueue_write( message.to_string() );

      lambdas_.emplace( make_pair( current_id_, connection ) );
      free_lambdas_.emplace( current_id_ );
      current_id_++;
      return true;
    }
  );

  cerr << "[meow] Listening for incoming connections on " << listen_addr_.str() << endl;
}

void MeowExecutionEngine::force_thunk( const Thunk &, ExecutionLoop & )
{
  cerr << "Not implemented" << endl;

  /* let's just launch one Lambda */
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

bool MeowExecutionEngine::can_execute( const gg::thunk::Thunk & thunk ) const
{
  return thunk.infiles_size() < 200_MiB;
}

size_t MeowExecutionEngine::job_count() const
{
  return 0;
}
