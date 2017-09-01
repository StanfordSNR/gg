/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "aws.hh"

#include <stdexcept>

#include "util.hh"

using namespace std;

#define AWS_ACCESS_KEY_ENV "AWS_ACCESS_KEY_ID"
#define AWS_SECRET_KEY_ENV "AWS_SECRET_ACCESS_KEY"

AWSCredentials::AWSCredentials()
  : AWSCredentials( safe_getenv( AWS_ACCESS_KEY_ENV ),
                    safe_getenv( AWS_SECRET_KEY_ENV ) )
{}

AWSCredentials::AWSCredentials( const string & access_key,
                                const string & secret_key )
  : access_key_( access_key ), secret_key_( secret_key )
{
  if ( access_key_.length() == 0 or secret_key_.length() == 0 )
  {
    throw runtime_error( "Missing AWS access key and/or secret key" );
  }
}
