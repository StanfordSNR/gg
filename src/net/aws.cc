/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "aws.hh"

#include <ctime>
#include <cassert>
#include <stdexcept>

#include "util/util.hh"

using namespace std;

const static string AWS_ACCESS_KEY_ENV { "AWS_ACCESS_KEY_ID" };
const static string AWS_SECRET_KEY_ENV { "AWS_SECRET_ACCESS_KEY" };
const static string AWS_SESSION_TOKEN_ENV { "AWS_SESSION_TOKEN" };

AWSRequest::AWSRequest( const AWSCredentials & credentials, const string & region,
                        const string & first_line, const string & contents )
  : credentials_( credentials ), region_( region ),
    request_date_( x_amz_date_( time( 0 ) ) ), first_line_( first_line ),
    contents_( contents ), headers_()
{
  headers_[ "x-amz-date" ] = request_date_;
}

string AWSRequest::x_amz_date_( const time_t & t )
{
  char sbuf[ 17 ];
  strftime( sbuf, 17, "%Y%m%dT%H%M%SZ", gmtime( &t ) );
  return string( sbuf, 16 );
}

HTTPRequest AWSRequest::to_http_request() const
{
  HTTPRequest req;

  req.set_first_line( first_line_ );
  for ( const auto & header : headers_ ) {
    req.add_header( HTTPHeader { header.first, header.second } );
  }
  req.done_with_headers();

  req.read_in_body( contents_ );
  assert( req.state() == COMPLETE );

  return req;
}

AWSCredentials::AWSCredentials()
  : AWSCredentials( safe_getenv( AWS_ACCESS_KEY_ENV ),
                    safe_getenv( AWS_SECRET_KEY_ENV ),
                    safe_getenv_or( AWS_SESSION_TOKEN_ENV, {} ) )
{}

AWSCredentials::AWSCredentials( const string & access_key,
                                const string & secret_key,
                                const string & session_token )
  : access_key_( access_key ), secret_key_( secret_key ),
    session_token_( session_token.length() > 0, session_token )
{
  if ( access_key_.length() == 0 or secret_key_.length() == 0 )
  {
    throw runtime_error( "Missing AWS access key and/or secret key" );
  }
}
