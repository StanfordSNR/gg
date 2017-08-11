/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "digest.hh"

#include <stdexcept>
#include <sstream>
#include <iomanip>

using namespace std;
using namespace digest;

template<const EVP_MD * evp_func(void)>
void Digest<evp_func>::update( istream & input )
{
  if ( finalized_) {
    throw runtime_error( "cannot update a finalized digest" );
  }

  char buf[ 1024 * 16 ];
  while ( input.good() ) {
    input.read( buf, sizeof( buf ) );
    EVP_DigestUpdate( &context_, buf, input.gcount() );
  }
}

template<const EVP_MD * evp_func(void)>
Digest<evp_func>::Digest()
  : context_()
{
  EVP_DigestInit( &context_, evp_func() );
}

template<const EVP_MD * evp_func(void)>
Digest<evp_func>::Digest( istream & input )
  : Digest()
{
  update( input );
}

template<const EVP_MD * evp_func(void)>
void Digest<evp_func>::update( const string & input )
{
  if ( finalized_) {
    throw runtime_error( "cannot update a finalized digest" );
  }

  EVP_DigestUpdate( &context_, input.data(), input.length() );
}

template<const EVP_MD * evp_func(void)>
void Digest<evp_func>::finalize()
{
  if ( finalized_ ) {
    return;
  }

  unsigned int len;
  EVP_DigestFinal( &context_, hash_, &len );
  finalized_ = true;
}

template<const EVP_MD * evp_func(void)>
string Digest<evp_func>::hexdigest()
{
  if ( not finalized_ ) {
    finalize();
  }

  stringstream result;

  result << hex << nouppercase << setfill('0');
  size_t digest_size = EVP_MD_size( evp_func() );

  for ( size_t i = 0; i < digest_size; i++ ) {
    result << setw( 2 ) << ( int )hash_[ i ];
  }

  return result.str();
}
