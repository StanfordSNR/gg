/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef DIGEST_HH
#define DIGEST_HH

#include <openssl/evp.h>
#include <istream>
#include <string>
#include <fstream>

namespace digest
{
  template<const EVP_MD * evp_func(void)>
  class Digest
  {
  private:
    bool finalized_ { false };

    unsigned char hash_[ EVP_MAX_MD_SIZE ];
    EVP_MD_CTX context_;
    unsigned int compute_hash( std::istream & input );

  public:
    Digest();
    Digest( std::istream & input );

    void update( const std::string & input );
    void update( std::istream & input );

    void finalize();

    std::string hexdigest();
  };

  template class Digest<EVP_sha256>;
  template class Digest<EVP_sha512>;

  using SHA256 = Digest<EVP_sha256>;
  using SHA512 = Digest<EVP_sha512>;
}

#endif /* DIGEST_HH */
