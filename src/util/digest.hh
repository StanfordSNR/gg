/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef DIGEST_HH
#define DIGEST_HH

#include <string>

namespace digest
{
  std::string sha256( const std::string & input );
}

#endif /* DIGEST_HH */
