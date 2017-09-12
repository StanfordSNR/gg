/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef REMOTE_HH
#define REMOTE_HH

#include <string>

#include "thunk.hh"
#include "lambda.hh"
#include "http_request.hh"
#include "aws.hh"

namespace lambda {

  class RequestGenerator
  {
  private:
    AWSCredentials credentials_;
    std::string region_;

  public:
    RequestGenerator( const AWSCredentials & credentials, const std::string & region );
    HTTPRequest generate( const gg::thunk::Thunk & thunk );
  };

}

#endif /* REMOTE_HH */
