/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "util.hh"

#include "protobufs/gg.pb.h"
#include "protobufs/util.hh"
#include "thunk/ggutils.hh"
#include "util/path.hh"
#include "thunk/thunk.hh"

using namespace std;
using namespace gg;
using namespace meow;
using namespace gg::thunk;

void meow::handle_message( const Message & message,
                           const shared_ptr<TCPConnection> & connection,
                           ExecutionLoop & )
{
  switch ( message.opcode() ) {
  case Message::OpCode::Hey:
    break;

  case Message::OpCode::Put:
  {
    const string & data = message.payload();
    ObjectType type = data.compare( 0, thunk::MAGIC_NUMBER.length(), thunk::MAGIC_NUMBER )
                      ? ObjectType::Value
                      : ObjectType::Thunk;

    const string hash = gg::hash::compute( data, type );
    roost::atomic_create( data, gg::paths::blob_path( hash ) );
    break;
  }

  case Message::OpCode::Get:
  {
    const string & hash = message.payload();
    Message response = create_put_message( hash );
    connection->enqueue_write( response.to_string() );
    break;
  }

  case Message::OpCode::Execute:
  {
    protobuf::RequestItem request;
    protoutil::from_json( message.payload(), request );

    cerr << "[meow] execute " << request.hash() << endl;
    break;
  }

  default:
    throw runtime_error( "not implemented" );
  }
}

Message meow::create_put_message( const string & hash )
{
  string requested_file = roost::read_file( gg::paths::blob_path( hash ) );
  return { Message::OpCode::Put, move( requested_file ) };
}

Message meow::create_execute_message( const gg::thunk::Thunk & thunk )
{
  string execution_payload = protoutil::to_json( Thunk::execution_request( thunk ) );
  return { Message::OpCode::Execute, move( execution_payload ) };
}
