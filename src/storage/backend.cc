/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "backend.hh"

#include <iostream>
#include <regex>
#include <stdexcept>

#include "backend_local.hh"
#include "backend_s3.hh"
#include "optional.hh"
#include "ggpaths.hh"

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
  }
  else {
    throw runtime_error( "malformed storage uri" );
  }

  if ( endpoint.protocol == "s3" ) {
    AWSCredentials credentials;

    if ( endpoint.username.length() or endpoint.password.length() ) {
      credentials = AWSCredentials { endpoint.username, endpoint.password };
    }

    return make_unique<S3StorageBackend>( credentials, endpoint.host, gg::remote::s3_region() );
  }

  return {};
}
