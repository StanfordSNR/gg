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

    Address destination_addr { argv[ 1 ], static_cast<uint16_t>( port_argv ) };

    ExecutionLoop loop;
    pair<uint64_t, ExecutionLoop::ConnectionIterator> main_connection;

    /* let's connect to the address */
    {
      TCPSocket socket;
      socket.set_blocking( false );
      socket.connect_nonblock( destination_addr );

      main_connection = move( loop.add_connection( "main-connection",
        [] ( const uint64_t, const string &, const HTTPResponse & ) -> bool
        {
          /* do something */
          return true;
        },
        [] ( const uint16_t, const string & )
        {
          /* do something */
        },
        socket
      ) );

      /* socket object is moved, so we end the block here to make sure
      no one accesses socket anymore */
    }

    while ( true ) {
      if ( loop.loop_once( -1 ).result == Poller::Result::Type::Exit ) {
        break;
      }
    }
  }
  catch ( const exception & e ) {
    print_exception( argv[ 0 ], e );
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
