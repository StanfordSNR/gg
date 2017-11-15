/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "backend_s3.hh"

using namespace std;

S3StorageBackend::S3StorageBackend( const AWSCredentials & credentials,
                                    const string & s3_bucket,
                                    const string & s3_region )
  : client_( credentials, { s3_region } ), bucket_( s3_bucket )
{}
