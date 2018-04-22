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

    PutRequest( const roost::path & filename,
                const std::string & object_key,
                const std::string & content_hash )
      : filename( filename ), object_key( object_key ),
        content_hash( true, content_hash ) {}

    PutRequest( const roost::path & filename,
                const std::string & object_key )
      : filename( filename ), object_key( object_key ),
        content_hash( false ) {}
  };

  struct GetRequest
  {
    std::string object_key;
    roost::path filename;
    Optional<mode_t> mode { false };

    GetRequest( const std::string & object_key,
                const roost::path & filename )
      : object_key( object_key ), filename( filename ) {}

    GetRequest( const std::string & object_key,
                const roost::path & filename,
                const mode_t mode )
      : object_key( object_key ), filename( filename ), mode( true, mode ) {}
  };

}

#endif /* STORAGE_REQUESTS_HH */
