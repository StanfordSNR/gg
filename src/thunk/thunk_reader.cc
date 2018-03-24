/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <fcntl.h>

#include "thunk/thunk_reader.hh"
#include "util/exception.hh"
#include "util/file_descriptor.hh"
#include "util/serialization.hh"

using namespace std;
using namespace gg;
using namespace gg::thunk;

bool ThunkReader::is_thunk( const string & path )
{
  FileDescriptor file { CheckSystemCall( "open (" + path + ")",
                                         open( path.c_str(), O_RDONLY ) ) };
  string magic = file.read_exactly( MAGIC_NUMBER.size(), true );
  return magic == MAGIC_NUMBER;
}

bool ThunkReader::is_thunk( const roost::path & path )
{
  return ThunkReader::is_thunk( path.string() );
}

Thunk ThunkReader::read( const std::string & path, const std::string & hash )
{
  ProtobufDeserializer deserializer { path };
  protobuf::Thunk thunk_proto;

  deserializer.read_string( MAGIC_NUMBER.length() ); /* skipping the magic number */
  deserializer.read_protobuf( thunk_proto );

  Thunk thunk { thunk_proto };

  if ( hash.length() > 0 ) {
    thunk.set_hash( hash );
  }

  return thunk;
}

Thunk ThunkReader::read( const roost::path & path, const std::string & hash )
{
  return move( ThunkReader::read( path.string(), hash ) );
}
