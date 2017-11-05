/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef DIGEST_HH
#define DIGEST_HH

#include <string>

namespace digest
{
  constexpr size_t length = 256 / 6 /* base64(sha256) */ + 1 /* round up */ + 8 /* length */;
  std::string sha256( const std::string & input, const bool exec_hash = false );
  std::string gghash_to_hex( const std::string & input );
}

namespace base64
{
  std::string encode( const std::string & input );
}

#endif /* DIGEST_HH */
