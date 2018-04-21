/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <string>
#include <iostream>
#include <stdexcept>
#include <cstdlib>

#include "protobufs/gg.pb.h"
#include "protobufs/util.hh"
#include "net/address.hh"
#include "net/http_request.hh"
#include "net/http_response.hh"
#include "net/http_request_parser.hh"
#include "execution/loop.hh"
#include "thunk/ggutils.hh"
#include "thunk/thunk.hh"
#include "util/system_runner.hh"
#include "util/path.hh"
#include "util/base64.hh"

using namespace std;
using namespace gg;

void usage( char * argv0 )
{
  cerr << "Usage: " << argv0 << " IP PORT" << endl;
}

int main( int argc, char * argv[] )
{
  try {
    if ( argc <= 0 ) {
      abort();
    }

    if ( argc != 3 ) {
      usage( argv[ 0 ] );
      return EXIT_FAILURE;
    }

    int port_argv = stoi( argv[ 2 ] );

    if ( port_argv <= 0 or port_argv > numeric_limits<uint16_t>::max() ) {
      throw runtime_error( "invalid port" );
    }

    Address listen_addr { argv[ 1 ], static_cast<uint16_t>( port_argv ) };

    ExecutionLoop loop;

    loop.make_listener( listen_addr,
      [] ( ExecutionLoop & loop, TCPSocket && socket ) {
        /* an incoming connection! */

        auto request_parser = make_shared<HTTPRequestParser>();

        loop.add_connection<TCPSocket>( move( socket ),
          [request_parser, &loop] ( shared_ptr<TCPConnection> connection, string && data ) {
            request_parser->parse( move( data ) );

            while ( not request_parser->empty() ) {
              HTTPRequest http_request { move( request_parser->front() ) };
              request_parser->pop();

              protobuf::ExecutionRequest exec_request;
              protoutil::from_json( http_request.body(), exec_request );

              /* now we should execute the thunk */
              loop.add_child_process( "thunk-execution",
                [conn_weak=weak_ptr<TCPConnection>( connection ),
                 http_request=move( http_request ), exec_request]
                ( const uint64_t, const string &, const int status ) { /* success callback */
                  if ( conn_weak.expired() ) {
                    /* there's not connection left to the guy who requested this,
                       let's forget about it */
                    return;
                  }

                  protobuf::ExecutionResponse response;

                  for ( auto & request_item : exec_request.thunks() ) {
                    const string & hash = request_item.hash();

                    protobuf::ResponseItem execution_response;
                    execution_response.set_thunk_hash( request_item.hash() );

                    bool discard_rest = false;
                    for ( const auto & tag : request_item.outputs() ) {
                      protobuf::OutputItem output_item;
                      Optional<cache::ReductionResult> result = cache::check( gg::hash::for_output( hash, tag ) );

                      if ( not result.initialized() ) {
                        discard_rest = true;
                        break;
                      }

                      const auto output_path = paths::blob_path( result->hash );
                      const string output_data = ""; // base64::encode( roost::read_file( output_path ) );

                      output_item.set_tag( tag );
                      output_item.set_hash( result->hash );
                      output_item.set_size( roost::file_size( output_path ) );
                      output_item.set_executable( roost::is_executable( output_path ) );
                      output_item.set_data( output_data );

                      *execution_response.add_outputs() = output_item;
                    }

                    if ( discard_rest ) { break; }
                    *response.add_executed_thunks() = execution_response;
                  }

                  response.set_return_code( status );
                  response.set_stdout( "" );

                  const string response_json = protoutil::to_json( response );

                  HTTPResponse http_response;
                  http_response.set_request( http_request );
                  http_response.set_first_line( "HTTP/1.1 200 OK" );
                  http_response.add_header( HTTPHeader{ "Content-Length", to_string( response_json.size() ) } );
                  http_response.add_header( HTTPHeader{ "Content-Type", "application/octet-stream" } );
                  http_response.done_with_headers();
                  http_response.read_in_body( response_json );
                  assert( http_response.state() == COMPLETE );
                },
                [exec_request] () -> int { /* child process */
                  setenv( "GG_STORAGE_URI", exec_request.storage_backend().c_str(), true );

                  vector<string> command {
                    "gg-execute-static",
                    "--get-dependencies",
                    "--put-output",
                    // "--cleanup"
                  };

                  for ( auto & request_item : exec_request.thunks() ) {
                    roost::atomic_create( base64::decode( request_item.data() ),
                                          paths::blob_path( request_item.hash() ) );

                    command.emplace_back( request_item.hash() );
                  }

                  return ezexec( command[ 0 ], command, {}, true, true );
                },
                false
              );
            }

            return true;
          },
          [] () {
            cerr << "error" << endl;
          },
          [] () {
            cerr << "closed" << endl;
          }
        );

        return true;
      } );

    while ( true ) {
      loop.loop_once( -1 );
    }

  }
  catch ( const exception &  e ) {
    print_exception( argv[ 0 ], e );
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
