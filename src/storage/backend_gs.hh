/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef STORAGE_BACKEND_GS_HH
#define STORAGE_BACKEND_GS_HH

#include "backend.hh"
#include "net/gcloud.hh"
#include "net/s3.hh"

class GoogleStorageBackend : public StorageBackend
{
private:
  S3Client client_;
  std::string bucket_;

public:
  GoogleStorageBackend( const GoogleStorageCredentials & credentials,
                        const std::string & bucket );

  void put( const std::vector<storage::PutRequest> & requests,
            const PutCallback & success_callback = []( const storage::PutRequest & ){} ) override;

  void get( const std::vector<storage::GetRequest> & requests,
            const GetCallback & success_callback = []( const storage::GetRequest & ){} ) override;

};

#endif /* STORAGE_BACKEND_GS_HH */
