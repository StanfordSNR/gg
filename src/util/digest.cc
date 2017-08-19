/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <crypto++/sha.h>
#include <crypto++/hex.h>

#include "digest.hh"

using namespace CryptoPP;
using namespace std;

std::string digest::sha256( const string & input )
{
  SHA256 hash_function;
  string ret;

  /* Each stage of the Crypto++ pipeline will delete the pointer it owns
     (https://www.cryptopp.com/wiki/Pipelining) */

  StringSource s( input, true,
                  new HashFilter( hash_function,
                                  new HexEncoder( new StringSink( ret ) ) ) );

  return ret;
}
