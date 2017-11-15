/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef STORAGE_BACKEND_S3_HH
#define STORAGE_BACKEND_S3_HH

#include "backend.hh"
#include "aws.hh"
#include "s3.hh"

class S3StorageBackend : public StorageBackend
{
private:
  S3Client client_;
  std::string bucket_;

public:
  S3StorageBackend( const AWSCredentials & credentials,
                    const std::string & s3_bucket,
                    const std::string & s3_region );

  void put( const PutRequest & request ) override;
  void put( const std::vector<PutRequest> & requests,
            const PutCallback & success_callback = []( const PutRequest & ){} ) override;

  void get( const GetRequest & request ) override;
  void get( const std::vector<GetRequest> & requests,
            const GetCallback & success_callback = []( const GetRequest & ){} ) override;

};

#endif /* STORAGE_BACKEND_S3_HH */
