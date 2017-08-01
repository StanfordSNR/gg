/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "thunk_writer.hh"

#include <iostream>
#include <fstream>

#include "serialization.hh"

using namespace std;
using namespace gg;
using namespace gg::thunk;

bool ThunkWriter::write_thunk( const Thunk & thunk, const string & filename )
{
  ProtobufSerializer serializer { filename };
  serializer.write_string( MAGIC_NUMBER );
  serializer.write_protobuf<protobuf::Thunk>( thunk.to_protobuf() );

  return true;
}
