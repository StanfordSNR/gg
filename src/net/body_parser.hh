/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef BODY_PARSER_HH
#define BODY_PARSER_HH

#include <string>

class BodyParser
{
public:
    /* possible return values from body parser:
        - entire string belongs to body
        - only some of string (0 bytes to n bytes) belongs to body */

    virtual std::string::size_type read( const std::string & str ) = 0;

    /* does message become complete upon EOF in body? */
    virtual bool eof() const = 0;

    virtual ~BodyParser() {}
};

/* used for RFC 2616 4.4 "rule 5" responses -- terminated only by EOF */
class Rule5BodyParser : public BodyParser
{
public:
    /* all of buffer always belongs to body */
    std::string::size_type read( const std::string & ) override
    {
        return std::string::npos;
    }

    /* does message become complete upon EOF in body? */
    /* when there was no content-length header on a response, answer is yes */
    bool eof() const override
    {
        return true;
    }
};

#endif /* BODY_PARSER_HH */
