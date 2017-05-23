/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef DIGEST_HH
#define DIGEST_HH

#include <openssl/evp.h>
#include <istream>
#include <string>

namespace digest
{
  template<const EVP_MD * evp_func(void)>
  class Digest
  {
  private:
    unsigned char hash_[ EVP_MAX_MD_SIZE ];

  public:
    Digest( std::istream & input );
    std::string hexdigest() const;
  };

  template class Digest<EVP_sha256>;
  template class Digest<EVP_sha512>;

  using SHA256 = Digest<EVP_sha256>;
  using SHA512 = Digest<EVP_sha512>;
}

#endif /* DIGEST_HH */
