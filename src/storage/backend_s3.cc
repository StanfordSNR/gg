/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "backend_s3.hh"

using namespace std;
using namespace storage;

S3StorageBackend::S3StorageBackend( const AWSCredentials & credentials,
                                    const string & s3_bucket,
                                    const string & s3_region )
  : client_( credentials, { s3_region } ), bucket_( s3_bucket )
{}

void S3StorageBackend::put( const std::vector<PutRequest> & requests,
                            const PutCallback & success_callback )
{
  client_.upload_files( bucket_, requests, success_callback );
}

void S3StorageBackend::get( const std::vector<GetRequest> & requests,
                            const GetCallback & success_callback )
{
  client_.download_files( bucket_, requests, success_callback );
}
