/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef S3_HH
#define S3_HH

#include <ctime>
#include <string>
#include <vector>
#include <map>
#include <functional>

#include "aws.hh"
#include "http_request.hh"
#include "path.hh"
#include "optional.hh"

class S3
{
public:
  static std::string endpoint( const std::string & bucket );

  struct UploadRequest
  {
    roost::path filename;
    std::string object_key;
    Optional<std::string> content_hash;
  };

  struct DownloadRequest
  {
    std::string object_key;
    roost::path filename;
  };
};

class S3PutRequest : public AWSRequest
{
public:
  S3PutRequest( const AWSCredentials & credentials,
                const std::string & region, const std::string & bucket,
                const std::string & object, const std::string & contents,
                const std::string & content_hash = {} );
};

class S3GetRequest : public AWSRequest
{
public:
  S3GetRequest( const AWSCredentials & credentials,
                const std::string & region, const std::string & bucket,
                const std::string & object );
};

struct S3ClientConfig
{
  std::string region { "us-west-1" };
  size_t max_threads { 32 };
  size_t max_batch_size { 32 };
};

class S3Client
{
private:
  AWSCredentials credentials_;
  S3ClientConfig config_;

public:
  S3Client( const S3ClientConfig & config = {} );

  void download_file( const std::string & bucket,
                      const std::string & object,
                      const roost::path & filename );

  void upload_files( const std::string & bucket,
                     const std::vector<S3::UploadRequest> & upload_requests,
                     const std::function<void( const S3::UploadRequest & )> & success_callback
                       = []( const S3::UploadRequest & ){} );

  void download_files( const std::string & bucket,
                       const std::vector<S3::DownloadRequest> & download_requests,
                       const std::function<void( const S3::DownloadRequest & )> & success_callback
                         = []( const S3::DownloadRequest & ){} );
};

#endif /* S3_HH */
