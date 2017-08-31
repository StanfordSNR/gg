/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef HTTP_REQUEST_PARSER_HH
#define HTTP_REQUEST_PARSER_HH

#include "http_message_sequence.hh"
#include "http_request.hh"

class HTTPRequestParser : public HTTPMessageSequence<HTTPRequest>
{
private:
    void initialize_new_message() override {}
};

#endif /* HTTP_REQUEST_PARSER_HH */
