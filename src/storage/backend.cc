/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "backend.hh"

#include <iostream>
#include <regex>
#include <stdexcept>

#include "storage/backend_local.hh"
#include "storage/backend_s3.hh"
#include "storage/backend_redis.hh"
#include "util/optional.hh"
#include "util/tokenize.hh"

using namespace std;

unique_ptr<StorageBackend> StorageBackend::create_backend( const string & uri )
{
  const static regex uri_regex {
    R"RAWSTR(((s3)://)?(([^:\n\r]+):([^@\n\r]+)@)?(([^:/\n\r]+):?(\d*))/?([^?\n\r]+)?\??([^#\n\r]*)?#?([^\n\r]*))RAWSTR" };

  smatch uri_match_result;

  StorageEndpoint endpoint;

  if ( regex_match( uri, uri_match_result, uri_regex ) ) {
    endpoint.protocol = uri_match_result[ 2 ];
    endpoint.username = uri_match_result[ 4 ];
    endpoint.password = uri_match_result[ 5 ];
    endpoint.host = uri_match_result[ 7 ];
    endpoint.path = uri_match_result[ 9 ];

    if ( uri_match_result[ 8 ].length() ) {
      endpoint.port.reset ( stoul( uri_match_result[ 8 ] ) );
    }

    if ( uri_match_result[ 10 ].length() ) {
      for ( const string & token : split( uri_match_result[ 10 ], "&" ) ) {
        if ( token.length() == 0 ) continue;

        string::size_type eq_pos = token.find( '=' );
        if ( eq_pos != string::npos ) {
          endpoint.options[ token.substr( 0, eq_pos ) ] = token.substr( eq_pos + 1 );
        }
        else {
          endpoint.options[ token ] = {};
        }
      }
    }
  }
  else {
    throw runtime_error( "malformed storage uri" );
  }

  if ( endpoint.protocol == "s3" ) {
    AWSCredentials credentials;

    if ( endpoint.username.length() or endpoint.password.length() ) {
      credentials = AWSCredentials { endpoint.username, endpoint.password };
    }

    return make_unique<S3StorageBackend>( credentials, endpoint.host,
                                          endpoint.options.count( "region" ) ? endpoint.options[ "region" ]
                                                                             : "us-east-1" );
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
