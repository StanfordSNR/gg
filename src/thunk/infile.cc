/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <iostream>
#include <cctype>
#include <fstream>
#include <sstream>
#include <iomanip>

#include "optional.hh"
#include "thunk.hh"
#include "thunk_reader.hh"
#include "digest.hh"
#include "path.hh"

using namespace std;
using namespace gg;
using namespace gg::thunk;

InFile::Type type_from_protobuf( const int protobuf_type )
{
  switch ( protobuf_type ) {
  case gg::protobuf::InFile_Type_FILE:
    return InFile::Type::FILE;
  case gg::protobuf::InFile_Type_DUMMY_DIRECTORY:
    return InFile::Type::DUMMY_DIRECTORY;
  default:
    throw runtime_error( "invalid protobuf infile type" );
  }
}

gg::protobuf::InFile_Type type_to_protobuf( const InFile::Type type )
{
  switch ( type ) {
  case InFile::Type::FILE:
    return gg::protobuf::InFile_Type_FILE;
  case InFile::Type::DUMMY_DIRECTORY:
    return gg::protobuf::InFile_Type_DUMMY_DIRECTORY;
  default:
    throw runtime_error( "invalid infile type" );
  }
}

InFile::InFile( const string & filename )
  : InFile( filename, filename )
{}

InFile::InFile( const string & filename, const string & real_filename )
  : filename_( filename ), real_filename_( real_filename ),
    content_hash_( compute_hash( real_filename_ ) ), order_( compute_order( real_filename_ ) ),
    size_( compute_size( real_filename_ ) )
{}

InFile::InFile( const string & filename, const string & real_filename,
                const string & hash, const size_t order, const off_t size )
  : filename_( filename ), real_filename_( real_filename ), content_hash_( hash ),
    order_( order ), size_( size )
{}

InFile::InFile( const string & filename, const string & real_filename,
                const string & hash, const size_t order )
  : InFile( filename, real_filename, hash, order, compute_size( real_filename ) )
{}

InFile::InFile( const string & filename, const string & real_filename,
                const string & hash )
  : InFile( filename, real_filename, hash, compute_order( real_filename ),
            compute_size( real_filename ) )
{}

InFile::InFile( const protobuf::InFile & infile_proto )
  : filename_( infile_proto.filename() ), real_filename_( filename_ ),
    content_hash_( infile_proto.hash() ), order_( infile_proto.order() ),
    size_( infile_proto.size() ),
    type_( type_from_protobuf( infile_proto.type() ) )
{}

InFile::InFile( const std::string & filename, const Type type )
  : InFile( filename, filename, "", 0, 0 )
{
  type_ = type;

  if ( type == Type::FILE ) {
    content_hash_ = compute_hash( real_filename_ );
    order_ = compute_order( real_filename_ );
    size_ = compute_size( real_filename_ );
  }
}

size_t InFile::compute_order( const string & filename )
{
  ThunkReader thunk_reader { filename };

  // check if the file has the gg-thunk magic number
  if ( not thunk_reader.is_thunk() ) {
    /* not a thunk file, so the order is 0 */
    return 0;
  }
  else {
    return thunk_reader.read_thunk().order();
  }
}

string InFile::compute_hash( const string & filename )
{
  // TODO : Check if file exists!
  ifstream file( filename, ifstream::binary );

  if( file.fail() ){
    throw runtime_error( "File " + filename + " does not exist." );
  }

  return digest::SHA256( file ).hexdigest();
}

off_t InFile::compute_size( const string & filename )
{
  return roost::file_size( filename );
}

protobuf::InFile InFile::to_protobuf() const
{
  protobuf::InFile infile;

  infile.set_filename( filename_ );
  infile.set_type( type_to_protobuf( type_ ) );

  if ( type_ == Type::FILE ) {
    infile.set_size( size_ );
    infile.set_hash( content_hash_ );
    infile.set_order( order_ );
  }

  return infile;
}

bool InFile::operator==( const InFile & other ) const
{
  return ( filename_ == other.filename_ ) and
         ( content_hash_ == other.content_hash_ ) and
         ( order_ == other.order_ ) and
         ( size_ == other.size_ ) and
         ( type_ == other.type_ );
}
