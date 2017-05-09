/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "thunk_reader.hh"

#include <iostream>
#include <fstream>

#include "serialization.hh"

using namespace std;
using namespace gg;
using namespace gg::thunk;

Optional<Thunk> ThunkReader::read_thunk( const string & filename )
{
  protobuf::Thunk thunk_proto;

  ProtobufDeserializer deserializer( filename );
  if ( MAGIC_NUMBER != deserializer.read_string( MAGIC_NUMBER.length() ) ) {
    return {};
  }

  deserializer.read_protobuf( thunk_proto );

  return { thunk_proto };
}
