/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "optional.hh"
#include "thunk.hh"
#include "thunk_reader.hh"
#include "digest.hh"
#include "path.hh"
#include "placeholder.hh"
#include "exception.hh"

using namespace std;
using namespace gg;
using namespace gg::thunk;

InFile::Type type_from_protobuf( const int protobuf_type )
{
  switch ( protobuf_type ) {
  case gg::protobuf::InFile_Type_FILE:
    return InFile::Type::FILE;
  case gg::protobuf::InFile_Type_EXECUTABLE:
    return InFile::Type::EXECUTABLE;
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
  case InFile::Type::EXECUTABLE:
    return gg::protobuf::InFile_Type_EXECUTABLE;
  case InFile::Type::DUMMY_DIRECTORY:
    return gg::protobuf::InFile_Type_DUMMY_DIRECTORY;
  default:
    throw runtime_error( "invalid infile type" );
  }
}

InFile::InFile( const string & filename, const string & real_filename,
                const Type type )
  : InFile( filename, ( real_filename.empty() ? filename : real_filename ),
            "", 0, 0, type )
{
  type_ = type;

  if ( type_ != Type::DUMMY_DIRECTORY ) {
    fill_file_info();
  }
}

InFile::InFile( const string & filename, const string & real_filename,
                const string & hash, const size_t order, const off_t size,
                const Type type )
  : filename_( roost::path( filename ).lexically_normal().string() ),
    real_filename_( real_filename ), content_hash_( hash ), order_( order ),
    size_( size ), type_( type )
{}

InFile::InFile( const protobuf::InFile & infile_proto )
  : filename_( infile_proto.filename() ), real_filename_( filename_ ),
    content_hash_( infile_proto.hash() ), order_( infile_proto.order() ),
    size_( infile_proto.size() ),
    type_( type_from_protobuf( infile_proto.type() ) )
{}

void InFile::fill_file_info()
{
  Optional<ThunkPlaceholder> placeholder = ThunkPlaceholder::read( real_filename_ );

  if ( not placeholder.initialized() ) {
    content_hash_ = compute_hash( real_filename_ );
    order_ = compute_order( real_filename_ );
    size_ = compute_size( real_filename_ );
  }
  else {
    content_hash_ = placeholder->content_hash();
    order_ = placeholder->order();
    size_ = placeholder->size();
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
  FileDescriptor file { CheckSystemCall( "open (" + filename + ")",
                                         open( filename.c_str(), O_RDONLY ) ) };

  string contents;
  while ( not file.eof() ) {
    contents += file.read();
  }

  return digest::sha256( contents );
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

  if ( type_ != Type::DUMMY_DIRECTORY ) {
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
