/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "metadata.hh"

#include "protobufs/meta.pb.h"
#include "protobufs/util.hh"
#include "thunk/ggutils.hh"

using namespace std;
using namespace gg::protobuf;

PlaceholderMetadata::PlaceholderMetadata( const string & metadata_str )
{
  meta::Metadata proto;
  protoutil::from_json( metadata_str, proto );
  args_.insert( args_.end(), proto.args().begin(), proto.args().end() );
  cwd_ = proto.working_directory();

  for ( const auto & object : proto.dependencies() ) {
    objects_.emplace_back( object.filename(), object.filename(),
                           gg::hash::type( object.hash() ), object.hash() );
  }
}

void PlaceholderMetadata::add_object( const ThunkFactory::Data & object )
{
  objects_.emplace_back( object );
}

std::string PlaceholderMetadata::str() const
{
  meta::Metadata proto;
  proto.set_working_directory( cwd_ );
  *proto.mutable_args() = { args_.begin(), args_.end() };

  for ( const auto & object : objects_ ) {
    meta::Object object_proto;
    object_proto.set_hash( object.hash() );
    object_proto.set_filename( object.real_filename() );
    *proto.add_dependencies() = object_proto;
  }

  return protoutil::to_json( proto );
}
