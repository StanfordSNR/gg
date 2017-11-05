/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <sstream>
#include <iomanip>
#include <algorithm>
#include <crypto++/sha.h>
#include <crypto++/hex.h>
#include <crypto++/base64.h>

#include "digest.hh"

using namespace CryptoPP;
using namespace std;

string digest::sha256( const string & input, const bool exec_hash )
{
  SHA256 hash_function;
  string ret;
  ostringstream output_sstr;

  /* Each stage of the Crypto++ pipeline will delete the pointer it owns
     (https://www.cryptopp.com/wiki/Pipelining) */

  StringSource s( input, true,
                  new HashFilter( hash_function,
                                  new Base64URLEncoder( new StringSink( ret ), false ) ) );

  if ( not exec_hash ) {
    replace( ret.begin(), ret.end(), '-', '.' );
    output_sstr << ret << setfill( '0' ) << setw( 8 ) << hex << input.length();
    return output_sstr.str();
  }
  else {
    return ret;
  }
}

string digest::gghash_to_hex( const string & input )
{
  string output;

  string hash = input.substr( 0, input.length() - 8 );
  replace( hash.begin(), hash.end(), '.', '-' );
  hash += '=';

  StringSource s( hash, true,
                  new Base64URLDecoder( new HexEncoder( new StringSink( output ), false ) ) );

  if ( output.length() == 64 ) {
    return output;
  }
  else {
    throw runtime_error( "invalid gghash: " + input );
  }
}

string base64::encode( const string & input )
{
  string output;
  StringSource s( input, true,
                  new Base64Encoder( new StringSink( output ), false ) );
  return output;
}
