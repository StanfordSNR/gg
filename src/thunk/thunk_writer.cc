/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "thunk_writer.hh"

#include <iostream>
#include <fstream>

#include "serialization.hh"
#include "ggpaths.hh"
#include "path.hh"
#include "temp_file.hh"

using namespace std;
using namespace gg;
using namespace gg::thunk;

std::string ThunkWriter::write_thunk( const Thunk & thunk )
{
  TempFile temp_thunk { paths::blob_path( "TEMPTHUNK" ).string() };

  write_thunk( thunk, temp_thunk.name() );
  string thunk_hash = InFile::compute_hash( temp_thunk.name() );
  roost::path thunk_in_gg_path = paths::blob_path( thunk_hash );

  if ( not roost::exists( thunk_in_gg_path ) ) {
    roost::rename( temp_thunk.name(), thunk_in_gg_path );
  }

  return thunk_hash;
}

void ThunkWriter::write_thunk( const Thunk & thunk, const string & filename )
{
  ProtobufSerializer serializer { filename };
  serializer.write_string( MAGIC_NUMBER );
  serializer.write_protobuf<protobuf::Thunk>( thunk.to_protobuf() );
}
