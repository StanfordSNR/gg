/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "engine_meow.hh"

#include <iostream>

#include "util/units.hh"
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
    [] ( ExecutionLoop & loop, shared_ptr<TCPConnection> & connection ) {
      cerr << "incoming connection: "
           << connection->socket().peer_address().str() << endl;

      loop.add_connection( connection,
        [] ( const string & data ) {
          cerr << "data received: " << data << endl;
          return true;
        },
        [] () {
          cerr << "Error occured!" << endl;
        },
        [] () {
          cerr << "Connection closed." << endl;
        }
      );

      return true;
    }
  );

  cerr << "[meow] listening for incoming connections on " << listen_addr_.str() << endl;
}

void MeowExecutionEngine::force_thunk( const Thunk &, ExecutionLoop & loop )
{
  cerr << "Not implemented" << endl;

  /* let's just launch one Lambda */
  loop.make_http_request<SSLConnection>( "start-worker", aws_addr_,
    generate_request(),
    [] ( const uint64_t, const string &, const HTTPResponse & response ) {
      cerr << response.str() << endl;
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
  return 0;
}
