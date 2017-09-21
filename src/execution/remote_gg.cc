/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "remote_gg.hh"

#include <string>
#include <sstream>

#include "thunk.hh"
#include "ggpaths.hh"
#include "exception.hh"

using namespace std;
using namespace gg;
using namespace gg::thunk;
using namespace ggremote;

HTTPRequest ExecutionConnectionManager::generate_request( const Thunk & thunk,
                                                          const string & thunk_hash,
                                                          const bool timelog )
{
  string payload = thunk.execution_payload( thunk_hash, timelog );
  HTTPRequest request;
  request.set_first_line( "POST /cgi-bin/gg/execute.cgi HTTP/1.1" );
  request.add_header( HTTPHeader{ "Content-Length", to_string( payload.size() ) } );
  request.add_header( HTTPHeader{ "Host", "gg-run-server" } );
  request.done_with_headers();

  request.read_in_body( payload );
  assert( request.state() == COMPLETE );

  return request;
}

ExecutionConnectionManager::ExecutionConnectionManager( const std::string & address,
                                                        const uint16_t port )
  : address_( address, port )
{}

ConnectionContext & ExecutionConnectionManager::new_connection( const Thunk & thunk,
                                                                const string & hash )
{
  if ( connections_.count( hash ) > 0 ) {
    throw runtime_error( "hash already exists" );
  }

  HTTPRequest request = generate_request( thunk, hash, false );

  TCPSocket socket;
  socket.set_blocking( false );

  try {
    socket.connect( address_ );
    throw runtime_error( "nonblocking connect unexpectedly succeeded immediately" );
  } catch ( const unix_error & e ) {
    if ( e.error_code() == EINPROGRESS ) {
      /* do nothing */
    } else {
      throw;
    }
  }

  auto ret = connections_.emplace( piecewise_construct,
                                   forward_as_tuple( hash ),
                                   forward_as_tuple( move( socket ), move ( request ) ) );
  assert( ret.second );

  return ret.first->second;
}

void ExecutionConnectionManager::remove_connection( const std::string & hash )
{
  connections_.erase( hash );
}
