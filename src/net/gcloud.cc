/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "gcloud.hh"

#include <stdexcept>

#include "util/util.hh"

using namespace std;

const static string GCLOUD_ACCESS_KEY_ENV { "GCLOUD_ACCESS_KEY" };
const static string GCLOUD_SECRET_KEY_ENV { "GCLOUD_SECRET_KEY" };

GoogleStorageCredentials::GoogleStorageCredentials()
  : GoogleStorageCredentials( safe_getenv( GCLOUD_ACCESS_KEY_ENV ),
                              safe_getenv( GCLOUD_SECRET_KEY_ENV ) )
{}

GoogleStorageCredentials::GoogleStorageCredentials( const string & access_key,
                                                    const string & secret_key )
  : access_key_( access_key ), secret_key_( secret_key )
{
  if ( access_key_.length() == 0 or secret_key_.length() == 0 )
  {
    throw runtime_error( "Missing Google Cloud access key and/or secret key" );
  }
}
