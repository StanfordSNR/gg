/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "message.hh"

#include <iostream>
#include <stdexcept>
#include <endian.h>

#include "util/util.hh"

using namespace std;
using namespace meow;

string put_field( const uint32_t n )
{
  const uint32_t network_order = htobe32( n );
  return string( reinterpret_cast<const char *>( &network_order ),
                 sizeof( network_order ) );
}

/* avoid implicit conversions */
template<class T>
string put_field( T n ) = delete;

Message::Message( const Chunk & chunk )
{
  if ( chunk.size() < 5 ) {
    throw out_of_range( "incomplete header" );
  }
  payload_length_ = chunk( 0, 4 ).be32();
  opcode_ = static_cast<OpCode>( chunk( 4, 1 ).octet() );
  payload_ = chunk( 5 ).to_string();
}

Message::Message( const OpCode opcode, string && payload )
  : payload_length_( payload.length() ), opcode_( opcode ),
    payload_( move( payload ) )
{}

string Message::str() const
{
  string output;
  output += put_field( payload_length_ );
  output += to_underlying( opcode_ );
  output += payload_;

  return output;
}

uint32_t Message::expected_length( const Chunk & chunk )
{
  return 5 + ( ( chunk.size() < 5 ) ? 0 : chunk( 0, 4 ).be32() );
}

void MessageParser::parse( const string & buf )
{
  raw_buffer_.append( buf );

  while ( true ) {
    uint32_t expected_length = Message::expected_length( raw_buffer_ );

    if ( raw_buffer_.length() < expected_length ) {
      /* still need more bytes to have a complete message */
      break;
    }

    Message message { Chunk { reinterpret_cast<const uint8_t *>( raw_buffer_.data() ), expected_length } };
    raw_buffer_.erase( 0, expected_length );
    completed_messages_.emplace( move( message ) );
  }
}
