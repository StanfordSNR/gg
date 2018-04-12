/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "handler.hh"

#include "thunk/ggutils.hh"
#include "util/path.hh"

using namespace std;
using namespace gg;
using namespace meow;

void meow::handle_message( const Message & message,
                           const shared_ptr<TCPConnection> & connection )
{
  switch ( message.opcode() ) {
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
    string requested_file = roost::read_file( gg::paths::blob_path( hash ) );
    Message response { Message::OpCode::Put, move( requested_file ) };
    connection->enqueue_write( response.to_string() );
    break;
  }

  default:
    throw runtime_error( "not implemented" );
  }
}
