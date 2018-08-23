/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "uri.hh"

#include "tokenize.hh"

#include <regex>
#include <stdexcept>

using namespace std;

ParsedURI::ParsedURI( const std::string & uri )
{
  const static regex uri_regex {
    R"RAWSTR((([A-Za-z0-9]+)://)?(([^:\n\r]+):([^@\n\r]+)@)?(([^:/\n\r]+):?(\d*))/?([^?\n\r]+)?\??([^#\n\r]*)?#?([^\n\r]*))RAWSTR" };

  smatch uri_match_result;

  if ( regex_match( uri, uri_match_result, uri_regex ) ) {
    protocol = uri_match_result[ 2 ];
    username = uri_match_result[ 4 ];
    password = uri_match_result[ 5 ];
    host = uri_match_result[ 7 ];
    path = uri_match_result[ 9 ];

    if ( uri_match_result[ 8 ].length() ) {
      port.reset ( stoul( uri_match_result[ 8 ] ) );
    }

    if ( uri_match_result[ 10 ].length() ) {
      for ( const string & token : split( uri_match_result[ 10 ], "&" ) ) {
        if ( token.length() == 0 ) continue;

        string::size_type eq_pos = token.find( '=' );
        if ( eq_pos != string::npos ) {
          options[ token.substr( 0, eq_pos ) ] = token.substr( eq_pos + 1 );
        }
        else {
          options[ token ] = {};
        }
      }
    }
  }
  else {
    throw runtime_error( "malformed storage uri" );
  }
}
