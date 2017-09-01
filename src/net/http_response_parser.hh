/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef HTTP_RESPONSE_PARSER_HH
#define HTTP_RESPONSE_PARSER_HH

#include "http_message_sequence.hh"
#include "http_response.hh"
#include "http_request.hh"

class HTTPResponseParser : public HTTPMessageSequence<HTTPResponse>
{
private:
    /* Need this to handle RFC 2616 section 4.4 rule 1 */
    std::queue<HTTPRequest> requests_ {};

    void initialize_new_message() override;

public:
    void new_request_arrived( const HTTPRequest & request );
    unsigned int pending_requests() const { return requests_.size(); }
};

#endif /* HTTP_RESPONSE_PARSER_HH */
