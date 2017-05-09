/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "thunk_reader.hh"

#include <iostream>
#include <fstream>

#include "serialization.hh"

using namespace std;
using namespace gg;

Thunk ThunkReader::read_thunk( const string & filename )
{
  ProtobufDeserializer deserializer( filename );
  protobuf::Thunk thunk_proto;
  deserializer.read_protobuf( thunk_proto );

  return { thunk_proto };
}
