/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "digest.hh"

#include <stdexcept>
#include <sstream>
#include <iomanip>

using namespace std;
using namespace digest;

template<const EVP_MD * evp_func(void)>
unsigned int Digest<evp_func>::compute_hash( std::istream & input )
{
  EVP_MD_CTX context;
  EVP_DigestInit( &context, evp_func() );

  char buf[ 1024 * 16 ];
  while ( input.good() ) {
    input.read( buf, sizeof( buf ) );
    EVP_DigestUpdate( &context, buf, input.gcount() );
  }

  unsigned int len;
  EVP_DigestFinal( &context, hash_, &len );

  return len;
}

template<const EVP_MD * evp_func(void)>
Digest<evp_func>::Digest( const std::string & filename )
{
  ifstream fin( filename, ios::binary );
  compute_hash( fin );
}

template<const EVP_MD * evp_func(void)>
Digest<evp_func>::Digest( istream & input )
{
  compute_hash( input );
}

template<const EVP_MD * evp_func(void)>
string Digest<evp_func>::hexdigest() const
{
  // TODO : Consider using a different object than string
  stringstream result;

  result << hex << nouppercase << setfill('0');
  size_t digest_size = EVP_MD_size( evp_func() );

  for ( size_t i = 0; i < digest_size; i++ ) {
    result << setw( 2 ) << ( int )hash_[ i ];
  }

  return result.str();
}
