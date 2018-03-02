/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef STORAGE_REQUESTS_HH
#define STORAGE_REQUESTS_HH

#include <string>

#include "util/optional.hh"
#include "util/path.hh"

namespace storage {

  struct PutRequest
  {
    roost::path filename;
    std::string object_key;
    Optional<std::string> content_hash;
  };

  struct GetRequest
  {
    std::string object_key;
    roost::path filename;
  };

}

#endif /* STORAGE_REQUESTS_HH */
