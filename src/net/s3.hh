/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef S3_HH
#define S3_HH

#include <ctime>
#include <string>
#include <unordered_map>

#include "http_request.hh"

class S3PutRequest
{
private:
  static std::string x_amz_date_( const std::time_t & t );

  std::string request_date_;
  std::string akid_;
  std::string secret_;
  std::string region_;
  std::string bucket_;
  std::string object_;
  std::string contents_;
  std::string first_line_;
  std::unordered_map<std::string, std::string> headers_ {};
  void add_authorization();

public:
  HTTPRequest to_http_request() const;

  S3PutRequest( const std::string & akid, const std::string & secret,
                const std::string & region, const std::string & bucket,
                const std::string & object, const std::string & contents );
};

#endif /* S3_HH */
