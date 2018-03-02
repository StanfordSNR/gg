/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <string>
#include <cassert>

#include "http_response_parser.hh"
#include "http_response.hh"
#include "util/exception.hh"

using namespace std;

void HTTPResponseParser::initialize_new_message()
{
    /* do we have a request that we can match this response up with? */
    if ( requests_.empty() ) {
        throw runtime_error( "HTTPResponseParser: response without matching request" );
    }

    message_in_progress_.set_request( requests_.front() );

    requests_.pop();
}

void HTTPResponseParser::new_request_arrived( const HTTPRequest & request )
{
    requests_.push( request );
}
