/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <iostream>
#include <openssl/sha.h>
#include <fstream>
#include <sstream>
#include <iomanip>

#include "optional.hh"
#include "thunk.hh"
#include "thunk_reader.hh"

using namespace std;
using namespace gg;
using namespace gg::thunk;

InFile::InFile( const string & filename )
  : filename_( filename ), hash_( compute_hash( filename ) ), order_( compute_order() )
{}

InFile::InFile( const std::string & filename, const std::string & hash, const size_t order )
  : filename_( filename ), hash_( hash ), order_( order )
{}

InFile::InFile( const protobuf::InFile & infile_proto )
  : filename_( infile_proto.filename() ), hash_( infile_proto.hash() ),
    order_( infile_proto.order() )
{}

size_t InFile::compute_order() const
{
  Optional<Thunk> thunk = ThunkReader::read_thunk( filename_ );

  // check if the file has the gg-thunk magic number
  if ( not thunk.initialized() ) {
    /* not a thunk file, so the order is 0 */
    return 0;
  }
  else {
    return thunk.get().order();
  }
}

string InFile::compute_hash( const string & filename )
{
  // TODO : Check if file exists!
  std::ifstream file( filename, std::ifstream::binary );
  if( file.fail() ){
    throw runtime_error( "File " + filename + " does not exist." );
  }
  SHA256_CTX md5Context;
  SHA256_Init( &md5Context );
  char buf[1024 * 16];
  while ( file.good() ) {
    file.read( buf, sizeof( buf ) );
    SHA256_Update( &md5Context, buf, file.gcount() );
  }
  unsigned char result[ SHA256_DIGEST_LENGTH ];
  SHA256_Final( result, &md5Context );

  // TODO : Consider using a different object than string
  std::stringstream md5string;
  md5string << std::hex << std::uppercase << std::setfill('0');
  for( const auto &byte: result ){
    md5string << std::setw( 2 ) << ( int )byte;
  }
  return md5string.str();
}

protobuf::InFile InFile::to_protobuf() const
{
  protobuf::InFile infile;

  infile.set_filename( filename_ );
  infile.set_hash( hash_ );
  infile.set_order( order_ );

  return infile;
}

bool InFile::operator==( const InFile & other ) const
{
  return ( filename_ == other.filename_ ) and
         ( hash_ == other.hash_ ) and
         ( order_ == other.order_ );
}
