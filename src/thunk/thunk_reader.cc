/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "thunk_reader.hh"

#include <iostream>
#include <fstream>

#include "serialization.hh"

using namespace std;
using namespace gg;
using namespace gg::thunk;

ThunkReader::ThunkReader( const string & filename )
  : deserializer_( filename ),
    is_thunk_( MAGIC_NUMBER == deserializer_.read_string( MAGIC_NUMBER.length() ) )
{}

Thunk ThunkReader::read_thunk()
{
  protobuf::Thunk thunk_proto;

  if ( not is_thunk() ) {
    throw runtime_error( "invalid thunk file" );
  }

  deserializer_.read_protobuf( thunk_proto );

  return { thunk_proto };
}
