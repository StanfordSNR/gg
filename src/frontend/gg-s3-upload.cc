/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <cstdlib>
#include <string>
#include <iostream>
#include <cassert>
#include <ctime>
#include <map>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <thread>

#include "socket.hh"
#include "secure_socket.hh"
#include "http_request.hh"
#include "awsv4_sig.hh"
#include "http_response_parser.hh"
#include "exception.hh"

using namespace std;

const string region = "us-west-1";
const string endpoint = "s3-" + region + ".amazonaws.com";
const size_t thread_count = 32;
const size_t maximum_batch_size = 32;

TCPSocket tcp_connection( const Address & address )
{
  TCPSocket sock;
  sock.connect( address );
  return sock;
}

class S3PutRequest
{
private:
  static std::string x_amz_date_( const std::time_t & t )
  {
    char sbuf[ 17 ];
    strftime( sbuf, 17, "%Y%m%dT%H%M%SZ", gmtime( &t ) );
    return string( sbuf, 16 );
  }

  std::string request_date_;
  std::string akid_, secret_, bucket_, object_, contents_;
  std::string first_line_;
  std::map<std::string, std::string> headers_ {};
  void add_authorization();

public:
  HTTPRequest to_http_request() const
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

  S3PutRequest( const std::string & akid,
                const std::string & secret,
                const std::string & bucket,
                const std::string & object,
                const std::string & contents )
    : request_date_( x_amz_date_( time( 0 ) ) ),
      akid_( akid ), secret_( secret ),
      bucket_( bucket ), object_( object ),
      contents_( contents ),
      first_line_( "PUT /" + object + " HTTP/1.1" )
  {
    headers_[ "x-amz-date" ] = request_date_;
    headers_[ "host" ] = bucket + ".s3.amazonaws.com";
    headers_[ "content-length" ] = to_string( contents.length() );
    AWSv4Sig::sign_request( "PUT\n/" + object,
                            secret_, akid_, region, "s3", request_date_, contents,
                            headers_ );
  }
};

int main()
{
  const char * akid_cstr = getenv( "AWS_ACCESS_KEY_ID" );
  const char * secret_cstr = getenv( "AWS_SECRET_ACCESS_KEY" );

  if ( akid_cstr == nullptr or secret_cstr == nullptr ) {
    cerr << "Missing required environment variable: AWS_ACCESS_KEY_ID and AWS_SECRET_ACCESS_KEY\n";
    return EXIT_FAILURE;
  }

  vector<string> filenames;
  string filename;
  while ( cin >> filename ) {
    filenames.push_back( filename );
  }

  const Address s3_address { endpoint, "https" };

  vector<thread> threads;
  for ( size_t thread_index = 0; thread_index < thread_count; thread_index++ ) {
    if ( thread_index < filenames.size() ) {
      threads.emplace_back(
        [&] ( const size_t index )
        {
          unsigned int connections = 0, files = 0;
          // cerr << "thread " << index << " started.\n";
          for ( size_t first_file_in_batch = index; first_file_in_batch < filenames.size(); first_file_in_batch += thread_count * maximum_batch_size ) {
            // cerr << "thread " << index << " starting batch " << first_file_in_batch << "\n";
            SSLContext ssl_context;
            HTTPResponseParser responses;
            SecureSocket s3 = ssl_context.new_secure_socket( tcp_connection( s3_address ) );

            s3.connect();

            connections++;

            for ( size_t file_id = first_file_in_batch; file_id < min( filenames.size(), first_file_in_batch + thread_count * maximum_batch_size ); file_id += thread_count ) {
              // cerr << "thread " << index << " uploading file " << file_id << "\n";
              string filename = filenames.at( file_id );

              string contents;
              FileDescriptor file { CheckSystemCall( "open " + filename, open( filename.c_str(), O_RDONLY ) ) };
              while ( not file.eof() ) { contents.append( file.read() ); }
              file.close();

              S3PutRequest request( akid_cstr, secret_cstr, "ggfunbucket", filename, contents );
              HTTPRequest outgoing_request = request.to_http_request();
              responses.new_request_arrived( outgoing_request );

              s3.write( outgoing_request.str() );
              files++;
            }

            // cerr << "+";

            while ( responses.pending_requests() ) {
              /* drain responses */
              responses.parse( s3.read() );
              if ( not responses.empty() ) {
                if ( responses.front().first_line() != "HTTP/1.1 200 OK" ) {
                  cerr << "HTTP failure: " << responses.front().first_line() << "\n";
                  throw runtime_error( "HTTP failure" );
                }

                responses.pop();
              }
            }
            cerr << "thread " << index << " made " << connections << " connections and uploaded " << files << " files.\n";
            // cerr << "\b \b";
          }
        }, thread_index
      );
    }
  }

  for ( auto & thread : threads ) {
    thread.join();
  }

  return EXIT_SUCCESS;
}
