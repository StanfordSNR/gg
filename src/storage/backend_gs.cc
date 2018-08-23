/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "backend_gs.hh"

using namespace std;
using namespace storage;

GoogleStorageBackend::GoogleStorageBackend( const GoogleStorageCredentials & credentials,
                                            const string & bucket )
  : client_( { credentials.access_key(), credentials.secret_key() },
             { "", bucket + ".storage.googleapis.com", 32, 1 } ), bucket_( bucket )
{}

void GoogleStorageBackend::put( const std::vector<PutRequest> & requests,
                                const PutCallback & success_callback )
{
  client_.upload_files( bucket_, requests, success_callback );
}

void GoogleStorageBackend::get( const std::vector<GetRequest> & requests,
                                const GetCallback & success_callback )
{
  client_.download_files( bucket_, requests, success_callback );
}
