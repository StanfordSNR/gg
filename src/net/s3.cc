/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "s3.hh"

#include <cassert>
#include <thread>
#include <fcntl.h>
#include <sys/types.h>

#include "socket.hh"
#include "secure_socket.hh"
#include "http_request.hh"
#include "http_response_parser.hh"
#include "awsv4_sig.hh"
#include "exception.hh"

using namespace std;

const static std::string UNSIGNED_PAYLOAD = "UNSIGNED-PAYLOAD";

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
                            const string & contents,
                            const string & content_hash )
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
                          headers_, content_hash );
}

TCPSocket tcp_connection( const Address & address )
{
  TCPSocket sock;
  sock.connect( address );
  return sock;
}

S3Client::S3Client( const S3ClientConfig & config )
  : credentials_(), config_( config )
{}

void S3Client::upload_files( const string & bucket,
                             const vector<S3::UploadRequest> & upload_requests )
{
  const string endpoint = "s3-" + config_.region + ".amazonaws.com";
  const Address s3_address { endpoint, "https" };

  const size_t thread_count = config_.max_threads;
  const size_t batch_size = config_.max_batch_size;

  vector<thread> threads;
  for ( size_t thread_index = 0; thread_index < thread_count; thread_index++ ) {
    if ( thread_index < upload_requests.size() ) {
      threads.emplace_back(
        [&] ( const size_t index )
        {
          for ( size_t first_file_idx = index;
                first_file_idx < upload_requests.size();
                first_file_idx += thread_count * batch_size ) {
            SSLContext ssl_context;
            HTTPResponseParser responses;
            SecureSocket s3 = ssl_context.new_secure_socket( tcp_connection( s3_address ) );

            s3.connect();

            for ( size_t file_id = first_file_idx;
                  file_id < min( upload_requests.size(), first_file_idx + thread_count * batch_size );
                  file_id += thread_count ) {
              const string & filename = upload_requests.at( file_id ).filename.string();
              const string & object_key = upload_requests.at( file_id ).object_key;
              string hash = upload_requests.at( file_id ).content_hash.get_or( UNSIGNED_PAYLOAD );

              string contents;
              FileDescriptor file { CheckSystemCall( "open " + filename, open( filename.c_str(), O_RDONLY ) ) };
              while ( not file.eof() ) { contents.append( file.read() ); }
              file.close();

              S3PutRequest request { credentials_.access_key(),
                                     credentials_.secret_key(), config_.region,
                                     bucket, object_key, contents, hash };

              HTTPRequest outgoing_request = request.to_http_request();
              responses.new_request_arrived( outgoing_request );

              s3.write( outgoing_request.str() );
            }

            while ( responses.pending_requests() ) {
              /* drain responses */
              responses.parse( s3.read() );
              if ( not responses.empty() ) {
                if ( responses.front().first_line() != "HTTP/1.1 200 OK" ) {
                  throw runtime_error( "HTTP failure" );
                }

                responses.pop();
              }
            }
          }
        }, thread_index
      );
    }
  }

  for ( auto & thread : threads ) {
    thread.join();
  }
}
