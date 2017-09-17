/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef REMOTE_GG_HH
#define REMOTE_GG_HH

#include <sys/types.h>
#include <string>
#include <unordered_map>

#include "connection_context.hh"
#include "thunk.hh"
#include "http_request.hh"
#include "http_response_parser.hh"
#include "socket.hh"
#include "address.hh"

namespace ggremote {

  class ExecutionConnectionManager
  {
  private:
    Address address_;

    /* thunk_hash -> socket */
    std::unordered_map<std::string, ConnectionContext> connections_ {};

    HTTPRequest generate_request( const gg::thunk::Thunk & thunk,
                                  const std::string & thunk_hash,
                                  const bool timelog );

  public:
    ExecutionConnectionManager( const std::string & address );

    ConnectionContext & new_connection( const gg::thunk::Thunk & thunk,
                                         const std::string & hash );

    ConnectionContext & connection_context( const std::string & hash ) { return connections_.at( hash ); }

    void remove_connection( const std::string & hash );
  };

}

#endif /* REMOTE_GG_HH */
