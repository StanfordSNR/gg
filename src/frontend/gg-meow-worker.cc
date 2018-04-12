/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <stdexcept>
#include <cstdlib>

#include "net/address.hh"
#include "net/http_response.hh"
#include "net/http_request.hh"
#include "execution/loop.hh"
#include "execution/meow/message.hh"
#include "execution/meow/util.hh"
#include "util/exception.hh"
#include "util/util.hh"

using namespace std;

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

    /* let's make a connection back to the coordinator */
    meow::MessageParser message_parser;

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
        const meow::Message & message = message_parser.front();
        cerr << "[msg,opcode=" << static_cast<uint32_t>( message.opcode() ) << "]" << endl;
        meow::handle_message( message, connection );
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
