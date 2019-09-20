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
#include "util/exception.hh"
#include "util/temp_file.hh"

using namespace std;
using namespace storage;

const static std::string UNSIGNED_PAYLOAD = "UNSIGNED-PAYLOAD";

std::string S3::endpoint( const string & region, const string & bucket )
{
  if ( region == "us-east-1" ) {
    return bucket + ".s3.amazonaws.com";
  }
  else {
    return bucket + ".s3-" + region + ".amazonaws.com";
  }
}

S3PutRequest::S3PutRequest( const AWSCredentials & credentials,
                            const string & endpoint, const string & region,
                            const string & object, const string & contents,
                            const string & content_hash )
  : AWSRequest( credentials, region, "PUT /" + object + " HTTP/1.1", contents )
{
  headers_[ "host" ] = endpoint;
  headers_[ "content-length" ] = to_string( contents.length() );

  if ( credentials.session_token().initialized() ) {
    headers_[ "x-amz-security-token" ] = *credentials.session_token();
  }

  AWSv4Sig::sign_request( "PUT\n/" + object,
                          credentials_.secret_key(), credentials_.access_key(),
                          region_, "s3", request_date_, contents, headers_,
                          content_hash );
}

S3GetRequest::S3GetRequest( const AWSCredentials & credentials,
                            const string & endpoint, const string & region,
                            const string & object )
  : AWSRequest( credentials, region, "GET /" + object + " HTTP/1.1", {} )
{
  headers_[ "host" ] = endpoint;

  if ( credentials.session_token().initialized() ) {
    headers_[ "x-amz-security-token" ] = *credentials.session_token();
  }

  AWSv4Sig::sign_request( "GET\n/" + object,
                          credentials_.secret_key(), credentials_.access_key(),
                          region_, "s3", request_date_, {}, headers_,
                          {} );
}

TCPSocket tcp_connection( const Address & address )
{
  TCPSocket sock;
  sock.connect( address );
  return sock;
}

S3Client::S3Client( const AWSCredentials & credentials,
                    const S3ClientConfig & config )
  : credentials_( credentials ), config_( config )
{}

void S3Client::download_file( const string & bucket, const string & object,
                              const roost::path & filename )
{
  const string endpoint = ( config_.endpoint.length() > 0 )
                          ? config_.endpoint : S3::endpoint( config_.region, bucket );
  const Address s3_address { endpoint, "https" };

  SSLContext ssl_context;
  HTTPResponseParser responses;
  SecureSocket s3 = ssl_context.new_secure_socket( tcp_connection( s3_address ) );
  s3.connect();

  S3GetRequest request { credentials_, endpoint, config_.region, object };
  HTTPRequest outgoing_request = request.to_http_request();
  responses.new_request_arrived( outgoing_request );
  s3.write( outgoing_request.str() );

  FileDescriptor file { CheckSystemCall( "open",
    open( filename.string().c_str(), O_RDWR | O_TRUNC | O_CREAT,
          S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH ) ) };

  while ( responses.empty() ) {
    responses.parse( s3.read() );
  }

  if ( responses.front().first_line() != "HTTP/1.1 200 OK" ) {
    throw runtime_error( "HTTP failure in S3Client::download_file( " + bucket + ", " + object + " ): " + responses.front().first_line() );
  }
  else {
    file.write( responses.front().body(), true );
  }
}

void S3Client::upload_files( const string & bucket,
                             const vector<PutRequest> & upload_requests,
                             const function<void( const PutRequest & )> & success_callback )
{
  const string endpoint = ( config_.endpoint.length() > 0 )
                          ? config_.endpoint : S3::endpoint( config_.region, bucket );
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

              S3PutRequest request { credentials_, endpoint, config_.region,
                                     object_key, contents, hash };

              HTTPRequest outgoing_request = request.to_http_request();
              responses.new_request_arrived( outgoing_request );

              s3.write( outgoing_request.str() );
            }

            size_t response_count = 0;

            while ( responses.pending_requests() ) {
              /* drain responses */
              responses.parse( s3.read() );
              if ( not responses.empty() ) {
                if ( responses.front().first_line() != "HTTP/1.1 200 OK" ) {
                  throw runtime_error( "HTTP failure in S3Client::upload_files(): " + responses.front().first_line() );
                }
                else {
                  const size_t response_index = first_file_idx + response_count * thread_count;
                  success_callback( upload_requests[ response_index ] );
                }

                responses.pop();
                response_count++;
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

void S3Client::download_files( const std::string & bucket,
                               const std::vector<storage::GetRequest> & download_requests,
                               const std::function<void( const storage::GetRequest & )> & success_callback )
{
  const string endpoint = ( config_.endpoint.length() > 0 )
                          ? config_.endpoint : S3::endpoint( config_.region, bucket );
  const Address s3_address { endpoint, "https" };

  const size_t thread_count = config_.max_threads;
  const size_t batch_size = config_.max_batch_size;

  vector<thread> threads;
  for ( size_t thread_index = 0; thread_index < thread_count; thread_index++ ) {
    if ( thread_index < download_requests.size() ) {
      threads.emplace_back(
        [&] ( const size_t index )
        {
          for ( size_t first_file_idx = index;
                first_file_idx < download_requests.size();
                first_file_idx += thread_count * batch_size ) {
            SSLContext ssl_context;
            HTTPResponseParser responses;
            SecureSocket s3 = ssl_context.new_secure_socket( tcp_connection( s3_address ) );

            s3.connect();

            size_t expected_responses = 0;

            for ( size_t file_id = first_file_idx;
                  file_id < min( download_requests.size(), first_file_idx + thread_count * batch_size );
                  file_id += thread_count ) {
              const string & object_key = download_requests.at( file_id ).object_key;

              S3GetRequest request { credentials_, endpoint, config_.region, object_key };

              HTTPRequest outgoing_request = request.to_http_request();
              responses.new_request_arrived( outgoing_request );

              s3.write( outgoing_request.str() );
              expected_responses++;
            }

            size_t response_count = 0;

            while ( response_count != expected_responses ) {
              /* drain responses */
              responses.parse( s3.read() );
              if ( not responses.empty() ) {
                if ( responses.front().first_line() != "HTTP/1.1 200 OK" ) {
                  const size_t response_index = first_file_idx + response_count * thread_count;
                  throw runtime_error( "HTTP failure in downloading '" +
                                       download_requests.at( response_index ).object_key +
                                       "': " + responses.front().first_line() );
                }
                else {
                  const size_t response_index = first_file_idx + response_count * thread_count;
                  const string & filename = download_requests.at( response_index ).filename.string();

                  roost::atomic_create( responses.front().body(), filename,
                                        download_requests[ response_index ].mode.initialized(),
                                        download_requests[ response_index ].mode.get_or( 0 ) );

                  success_callback( download_requests[ response_index ] );
                }

                responses.pop();
                response_count++;
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
