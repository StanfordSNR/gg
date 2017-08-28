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

std::string digest::sha256( const string & input )
{
  SHA256 hash_function;
  string ret;
  ostringstream output_sstr;

  /* Each stage of the Crypto++ pipeline will delete the pointer it owns
     (https://www.cryptopp.com/wiki/Pipelining) */

  StringSource s( input, true,
                  new HashFilter( hash_function,
                                  new Base64URLEncoder( new StringSink( ret ), false ) ) );

  replace( ret.begin(), ret.end(), '-', '.' );
  output_sstr << ret << setfill( '0' ) << setw( 8 ) << hex << input.length();

  return output_sstr.str();
}
