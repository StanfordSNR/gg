/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "engine_meow.hh"

#include <iostream>

#include "util/units.hh"

using namespace std;
using namespace gg::thunk;
using namespace PollerShortNames;

MeowExecutionEngine::MeowExecutionEngine( const AWSCredentials & credentials,
                                          const std::string & region,
                                          const Address & listen_addr )
  : credentials_( credentials ), region_( region ),
    aws_addr_( LambdaInvocationRequest::endpoint( region_ ), "https" ),
    listen_addr_( listen_addr ), listen_socket_()
{}

void MeowExecutionEngine::init( ExecutionLoop & loop )
{
  listen_socket_.set_blocking( false );
  listen_socket_.set_reuseaddr();
  listen_socket_.bind( listen_addr_ );
  listen_socket_.listen();

  loop.poller().add_action( Poller::Action( listen_socket_, Direction::In,
    [this, &loop]() -> ResultType
    {
      /* incoming connection. we accept this connection, and we will
      add this to the looper */
      TCPSocket socket { move( listen_socket_.accept() ) };
      auto conn_info = loop.add_connection( "meow-worker",
        [] ( const uint64_t, const string &, const HTTPResponse & ) -> bool
        {
          /* handle the incoming message from Lambda */
          return true; /* keep the connection open */
        },
        [] ( const uint64_t, const string & ) -> void
        {
          /* handle the failure */
        },
        socket
      );

      connections_.emplace( move( conn_info ) );

      return ResultType::Continue;
    } )
  );
}

void MeowExecutionEngine::force_thunk( const Thunk &, ExecutionLoop & )
{
  cerr << "Not implemented" << endl;
}

bool MeowExecutionEngine::can_execute( const gg::thunk::Thunk & thunk ) const
{
  return thunk.infiles_size() < 200_MiB;
}

size_t MeowExecutionEngine::job_count() const
{
  return 0;
}
