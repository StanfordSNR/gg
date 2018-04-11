/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "handler.hh"

#include "thunk/ggutils.hh"
#include "util/path.hh"

using namespace std;
using namespace gg;
using namespace meow;

void handle_put( const Message & message )
{
  assert( message.opcode() == Message::OpCode::Put );

  const string & data = message.payload();
  ObjectType type = data.compare( 0, thunk::MAGIC_NUMBER.length(), thunk::MAGIC_NUMBER )
                    ? ObjectType::Value
                    : ObjectType::Thunk;

  const string hash = gg::hash::compute( data, type );
  roost::atomic_create( data, gg::paths::blob_path( hash ) );  
}
