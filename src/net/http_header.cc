/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <string>
#include <assert.h>

#include "http_header.hh"
#include "util/exception.hh"

using namespace std;

/* parse a header line into a key and a value */
HTTPHeader::HTTPHeader( const string & buf )
  : key_(), value_()
{
    const string separator = ":";

    /* step 1: does buffer contain colon? */
    size_t colon_location = buf.find( separator );
    if ( colon_location == std::string::npos ) {
        fprintf( stderr, "Buffer: %s\n", buf.c_str() );
        throw runtime_error( "HTTPHeader: buffer does not contain colon" );
    }

    /* step 2: split buffer */
    key_ = buf.substr( 0, colon_location );
    string value_temp = buf.substr( colon_location + separator.size() );

    /* strip whitespace */
    size_t first_nonspace = value_temp.find_first_not_of( " " );
    if ( first_nonspace == std::string::npos ) { /* handle case where value is only space */
        value_ = value_temp;
    } else {
        value_ = value_temp.substr( first_nonspace );
    }
}

HTTPHeader::HTTPHeader( const string & key, const string & value )
    : key_( key ), value_( value )
{}
