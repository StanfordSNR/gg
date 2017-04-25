/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <iostream>
#include <openssl/sha.h>
#include <fstream>
#include <sstream>
#include <iomanip>

#include "infile_desc.hh"

using namespace std;

string InFileDescriptor::compute_hash( string filename ){
  std::ifstream file( filename, std::ifstream::binary );
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


InFileDescriptor::InFileDescriptor( string filename )
  : filename_( filename ), hash_( compute_hash( filename ) ), order_( 0 )
{}

InFileDescriptor::~InFileDescriptor()
{}

json::Object InFileDescriptor::to_json()
{
  json::Object j;
  j[ "filename" ] = json::String( filename_ );
  j[ "hash" ] = json::String( hash_ );
  j[ "order" ] = json::Number( order_ );
  return j;
}
