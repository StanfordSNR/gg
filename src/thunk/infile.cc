/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <iostream>
#include <openssl/sha.h>
#include <fstream>
#include <sstream>
#include <iomanip>

#include "thunk.hh"

using namespace std;
using namespace gg;

InFile::InFile( string filename )
  : filename_( filename ), hash_( compute_hash( filename ) ), order_( 0 )
{}

InFile::InFile( const protobuf::InFile & infile_proto )
  : filename_( infile_proto.filename() ), hash_( infile_proto.hash() ),
    order_( infile_proto.order() )
{}

string InFile::compute_hash( string filename ){
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
