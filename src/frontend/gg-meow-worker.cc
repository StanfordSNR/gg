/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <iostream>
#include <string>
#include <limits>
#include <stdexcept>
#include <cstdlib>

#include "net/address.hh"
#include "net/http_response.hh"
#include "net/http_request.hh"
#include "execution/loop.hh"
#include "execution/meow/message.hh"
#include "util/exception.hh"

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
    auto message_parser = make_shared<meow::MessageParser>();

    shared_ptr<TCPConnection> connection = loop.make_connection<TCPConnection>( coordinator_addr,
      [message_parser] ( string && data ) {
        message_parser->parse( data );

        if ( not message_parser->empty() ) {
          /* we got a message! */
          cerr << "==== MESSAGE ====" << endl;
          cerr << message_parser->front().payload() << endl;
          cerr << "=================" << endl;
          message_parser->pop();
        }

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
    }
  }
  catch ( const exception & e ) {
    print_exception( argv[ 0 ], e );
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
