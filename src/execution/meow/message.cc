/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "message.hh"

#include <stdexcept>

using namespace std;
using namespace meow;

Message::Message( const Chunk & chunk )
{
  if ( chunk.size() < 6 ) {
    throw out_of_range( "incomplete header" );
  }

  opcode_ = static_cast<OpCode>( chunk( 0, 1 ).octet() );
  payload_length_ = chunk( 1, 4 ).be32();
  payload_ = chunk( 5 ).to_string();
}

Message::Message( const OpCode opcode, string && payload )
  : opcode_( opcode ), payload_length_( payload.length() ),
    payload_( move( payload ) )
{}
