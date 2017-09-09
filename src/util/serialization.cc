/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "serialization.hh"

#include <fcntl.h>
#include <string>

#include "exception.hh"

using namespace std;

ProtobufDeserializer::ProtobufDeserializer( const string & filename )
  : fin_( CheckSystemCall( filename,
                           open( filename.c_str(), O_RDONLY, 0 ) ) )
{}

ProtobufDeserializer::ProtobufDeserializer( FileDescriptor && fd )
  : fin_( move( fd ) )
{}

string ProtobufDeserializer::read_string( const size_t size )
{
  string ret;
  google::protobuf::io::CodedInputStream coded_input { &raw_input_ };

  if ( not coded_input.ReadString( &ret, size ) ) {
    return "";
  }

  return ret;
}
