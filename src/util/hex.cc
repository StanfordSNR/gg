#include "hex.hh"

#include <crypto++/hex.h>

using namespace CryptoPP;
using namespace std;

string hex::encode( const string & input )
{
  string ret;

  /* Each stage of the Crypto++ pipeline will delete the pointer it owns
     (https://www.cryptopp.com/wiki/Pipelining) */

  StringSource s( input, true,
                  new HexEncoder( new StringSink( ret ) ) );

  return ret;
}
