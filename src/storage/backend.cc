/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "backend.hh"

#include <iostream>
#include <regex>
#include <stdexcept>

#include "storage/backend_local.hh"
#include "storage/backend_s3.hh"
#include "storage/backend_gs.hh"
#include "storage/backend_redis.hh"
#include "util/optional.hh"
#include "util/uri.hh"

using namespace std;

unique_ptr<StorageBackend> StorageBackend::create_backend( const string & uri )
{
  ParsedURI endpoint { uri };

  if ( endpoint.protocol == "s3" ) {
    return make_unique<S3StorageBackend>(
      ( endpoint.username.length() or endpoint.password.length() )
        ? AWSCredentials { endpoint.username, endpoint.password }
        : AWSCredentials {},
      endpoint.host,
      endpoint.options.count( "region" )
        ? endpoint.options[ "region" ]
        : "us-east-1" );
  }
  else if ( endpoint.protocol == "gs" ) {
    return make_unique<GoogleStorageBackend>(
      ( endpoint.username.length() or endpoint.password.length() )
        ? GoogleStorageCredentials { endpoint.username, endpoint.password }
        : GoogleStorageCredentials {},
      endpoint.host );
  }
  else if ( endpoint.protocol == "redis" ) {
    RedisClientConfig config;
    config.ip = endpoint.host;
    config.port = endpoint.port.get_or( config.port );
    config.username = endpoint.username;
    config.password = endpoint.password;

    return make_unique<RedisStorageBackend>( config );
  }
  else {
    throw runtime_error( "unknown storage backend" );
  }

  return {};
}
