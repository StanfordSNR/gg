/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "s3.hh"

#include <cassert>

#include "awsv4_sig.hh"

using namespace std;

string S3PutRequest::x_amz_date_( const time_t & t )
{
  char sbuf[ 17 ];
  strftime( sbuf, 17, "%Y%m%dT%H%M%SZ", gmtime( &t ) );
  return string( sbuf, 16 );
}

HTTPRequest S3PutRequest::to_http_request() const
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

S3PutRequest::S3PutRequest( const string & akid,
                            const string & secret,
                            const string & region,
                            const string & bucket,
                            const string & object,
                            const string & contents )
  : request_date_( x_amz_date_( time( 0 ) ) ),
    akid_( akid ), secret_( secret ), region_( region ),
    bucket_( bucket ), object_( object ), contents_( contents ),
    first_line_( "PUT /" + object + " HTTP/1.1" )
{
  headers_[ "x-amz-date" ] = request_date_;
  headers_[ "host" ] = bucket + ".s3.amazonaws.com";
  headers_[ "content-length" ] = to_string( contents.length() );
  AWSv4Sig::sign_request( "PUT\n/" + object,
                          secret_, akid_, region_, "s3", request_date_, contents,
                          headers_ );
}
