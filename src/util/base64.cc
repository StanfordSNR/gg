/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "base64.hh"

#include <crypto++/sha.h>
#include <crypto++/hex.h>
#include <crypto++/base64.h>

using namespace CryptoPP;
using namespace std;

string base64::encode( const string & input )
{
  SHA256 hash_function;
  string ret;

  /* Each stage of the Crypto++ pipeline will delete the pointer it owns
     (https://www.cryptopp.com/wiki/Pipelining) */

  StringSource s( input, true,
                  new Base64Encoder( new StringSink( ret ) ) );

  return ret;
}

string base64::decode( const string & input )
{
  SHA256 hash_function;
  string ret;

  /* Each stage of the Crypto++ pipeline will delete the pointer it owns
     (https://www.cryptopp.com/wiki/Pipelining) */

  StringSource s( input, true,
                  new Base64Decoder( new StringSink( ret ) ) );

  return ret;
}
