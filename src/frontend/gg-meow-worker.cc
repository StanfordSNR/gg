/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <stdexcept>
#include <cstdlib>

#include "protobufs/gg.pb.h"
#include "protobufs/util.hh"
#include "net/address.hh"
#include "net/http_response.hh"
#include "net/http_request.hh"
#include "thunk/ggutils.hh"
#include "execution/loop.hh"
#include "execution/meow/message.hh"
#include "execution/meow/util.hh"
#include "util/base64.hh"
#include "util/exception.hh"
#include "util/path.hh"
#include "util/system_runner.hh"
#include "util/util.hh"

using namespace std;
using namespace gg;
using namespace meow;

void usage( char * argv0 )
{
  cerr << "Usage: " << argv0 << " DESTINATION PORT" << endl;
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

    Address coordinator_addr { argv[ 1 ], static_cast<uint16_t>( port_argv ) };
    ExecutionLoop loop;

    MessageParser message_parser;
    /* let's make a connection back to the coordinator */
    shared_ptr<TCPConnection> connection = loop.make_connection<TCPConnection>( coordinator_addr,
      [&message_parser] ( string && data ) {
        message_parser.parse( data );
        return true;
      },
      [] () {
        cerr << "Error." << endl;
      },
      [] () {
        cerr << "Closed." << endl;
        exit( 0 );
      } );

    while( true ) {
      loop.loop_once( -1 );

      while ( not message_parser.empty() ) {
        const Message & message = message_parser.front();
        cerr << "[msg,opcode=" << static_cast<uint32_t>( message.opcode() ) << "]" << endl;

        switch ( message.opcode() ) {
        case Message::OpCode::Put:
          handle_put_message( message );
          break;

        case Message::OpCode::Execute:
        {
          protobuf::RequestItem execution_request;
          protoutil::from_json( message.payload(), execution_request );

          /* let's write the thunk to disk first */
          roost::atomic_create( base64::decode( execution_request.data() ),
                                gg::paths::blob_path( execution_request.hash() ) );

          /* making it cheaper to copy */
          execution_request.set_data( "" );

          /* now we can execute it */
          cerr << "[execute] " << execution_request.hash() << endl;
          loop.add_child_process( execution_request.hash(),
            [execution_request] ( const uint64_t, const string & ) { /* success callback */

            },
            [execution_request] ( const uint64_t, const string & ) { /* failure callback */

            },
            [hash=execution_request.hash()]()
            {
              vector<string> command { "gg-execute", "--fix-permissions", hash };
              return ezexec( command[ 0 ], command, {}, true, true );
            }
          );

          break;
        }

        default:
          throw runtime_error( "unhandled opcode" );
        }

        message_parser.pop();
      }
    }
  }
  catch ( const exception & e ) {
    print_exception( argv[ 0 ], e );
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
