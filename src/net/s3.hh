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
  struct UploadRequest
  {
    roost::path filename;
    std::string object_key;
    Optional<std::string> content_hash;
  };
};

class S3PutRequest : public AWSRequest
{
private:
  std::string bucket_;
  std::string object_;

public:
  S3PutRequest( const std::string & akid, const std::string & secret,
                const std::string & region, const std::string & bucket,
                const std::string & object, const std::string & contents,
                const std::string & content_hash = {} );
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

  /* `files` is a vector of pairs<path_to_file, object_key> */
  void upload_files( const std::string & bucket,
                     const std::vector<S3::UploadRequest> & upload_requests,
                     std::function<void( const S3::UploadRequest & )> && success_callback
                       = [](const S3::UploadRequest &){} );
};

#endif /* S3_HH */
