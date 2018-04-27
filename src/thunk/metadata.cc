/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "metadata.hh"

#include <sstream>

#include "protobufs/meta.pb.h"
#include "protobufs/util.hh"
#include "thunk/ggutils.hh"

using namespace std;
using namespace gg::protobuf;

/* Metadata format:
   line 0: current working directory
   line 1: <N> the number of arguments
   line 2..(N+1): arguments
   line (N+2)..(end): <HASH> <FILENAME>  */

PlaceholderMetadata::PlaceholderMetadata( const string & metadata_str )
{
  istringstream iss { metadata_str };
  string line;

  size_t index = 0;
  size_t arg_count = 0;

  while ( getline( iss, line ) ) {
    if ( index == 0 ) {
      cwd_ = line;
    }
    else if ( index == 1 ) {
      arg_count = stoul( line );
    }
    else if ( 1 < index and index < arg_count + 2 ) {
      args_.emplace_back( line );
    }
    else {
      const auto space_pos = line.find( ' ' );
      string hash = line.substr( 0, space_pos );
      string filename = line.substr( space_pos + 1 );
      objects_.emplace_back( move( filename ), "", gg::hash::type( hash ), hash );
    }

    index++;
  }
}

void PlaceholderMetadata::add_object( const ThunkFactory::Data & object )
{
  objects_.emplace_back( object );
}

std::string PlaceholderMetadata::str() const
{
  ostringstream oss;

  oss << cwd_ << endl;
  oss << args_.size() << endl;

  for ( const auto & arg : args_ ) {
    oss << arg << endl;
  }

  for ( const auto & object : objects_ ) {
    oss << object.hash() << " " << object.real_filename() << endl;
  }

  return oss.str();
}
