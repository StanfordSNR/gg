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

const bool timelog = ( getenv( "GG_EXECUTE_TIMELOG" ) != nullptr );

class ProgramFinished : public exception {};

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
      [&message_parser] ( shared_ptr<TCPConnection>, string && data ) {
        message_parser.parse( data );
        return true;
      },
      [] () {
        cerr << "Error." << endl;
      },
      [] () {
        throw ProgramFinished();
      } );

    Message hello_message { Message::OpCode::Hey, "" };
    connection->enqueue_write( hello_message.str() );

    while( loop.loop_once( -1 ).result == Poller::Result::Type::Success ) {
      while ( not message_parser.empty() ) {
        const Message & message = message_parser.front();

        switch ( message.opcode() ) {
        case Message::OpCode::Put:
        {
          const string hash = handle_put_message( message );
          cerr << "[put] " << hash << endl;
          break;
        }

        case Message::OpCode::Get:
        {
          const string & hash = message.payload();
          string object_data = roost::read_file( gg::paths::blob( hash ) );
          Message message { Message::OpCode::Put, move( object_data ) };
          connection->enqueue_write( message.str() );
          cerr << "[get] " << hash << endl;
          break;
        }

        case Message::OpCode::Execute:
        {
          protobuf::RequestItem execution_request;
          protoutil::from_string( message.payload(), execution_request );

          /* let's write the thunk to disk first */
          roost::atomic_create( base64::decode( execution_request.data() ),
                                gg::paths::blob( execution_request.hash() ) );

          /* making it cheaper to copy */
          execution_request.set_data( "" );

          /* now we can execute it */
          cerr << "[execute] " << execution_request.hash() << endl;
          loop.add_child_process( execution_request.hash(),
            [hash=execution_request.hash(), execution_request, &connection]
            ( const uint64_t, const string &, const int status ) mutable {
              if ( status ) {
                /* execution failed */
                Message message { Message::OpCode::ExecutionFailed, move( hash ) };
                connection->enqueue_write( message.str() );
                return;
              }

              const string & hash = execution_request.hash();
              protobuf::ResponseItem execution_response;
              execution_response.set_thunk_hash( execution_request.hash() );

              for ( const auto & tag : execution_request.outputs() ) {
                protobuf::OutputItem output_item;
                Optional<cache::ReductionResult> result = cache::check( gg::hash::for_output( hash, tag ) );

                if ( not result.initialized() ) {
                  throw runtime_error( "output not found" );
                }

                const auto output_path = paths::blob( result->hash );
                const string output_data = ""; // base64::encode( roost::read_file( output_path ) );

                output_item.set_tag( tag );
                output_item.set_hash( result->hash );
                output_item.set_size( roost::file_size( output_path ) );
                output_item.set_executable( roost::is_executable( output_path ) );
                output_item.set_data( output_data );

                *execution_response.add_outputs() = output_item;
              }

              Message message { Message::OpCode::Executed, protoutil::to_string( execution_response ) };
              connection->enqueue_write( message.str() );
            },
            [hash=execution_request.hash()]()
            {
              vector<string> command { "gg-execute-static",
                                       "--get-dependencies",
                                       "--put-output",
                                       "--cleanup",
                                       hash };

              if ( timelog ) {
                command.push_back( "--timelog" );
              }

              return ezexec( command[ 0 ], command, {}, true, true );
            },
            false
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
  catch ( const ProgramFinished & ) {
    return EXIT_SUCCESS;
  }
  catch ( const exception & e ) {
    print_exception( argv[ 0 ], e );
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
