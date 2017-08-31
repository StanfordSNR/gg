/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef HTTP_MESSAGE_SEQUENCE
#define HTTP_MESSAGE_SEQUENCE

#include <string>
#include <queue>
#include <cassert>

#include "http_message.hh"

template <class MessageType>
class HTTPMessageSequence
{
private:
    class InternalBuffer
    {
    private:
        std::string buffer_ {};
    
    public:
        bool have_complete_line() const;

        std::string get_and_pop_line();

        void pop_bytes( const size_t n );

        bool empty() const { return buffer_.empty(); }

        void append( const std::string & str ) { buffer_.append( str ); }

        const std::string & str() const { return buffer_; }
    };

    /* bytes that haven't been parsed yet */
    InternalBuffer buffer_ {};

    /* complete messages ready to go */
    std::queue<MessageType> complete_messages_ {};

    /* one loop through the parser */
    /* returns whether to continue */
    bool parsing_step();

    /* what to do to create a new message.
       must be implemented by subclass */
    virtual void initialize_new_message() = 0;

protected:
    /* the current message we're working on */
    MessageType message_in_progress_ {};

public:
    HTTPMessageSequence() {}
    virtual ~HTTPMessageSequence() {}

    /* must accept all of buf */
    void parse( const std::string & buf );

    /* getters */
    bool empty() const { return complete_messages_.empty(); }
    const MessageType & front() const { return complete_messages_.front(); }

    /* pop one request */
    void pop() { complete_messages_.pop(); }
};

template <class MessageType>
bool HTTPMessageSequence<MessageType>::InternalBuffer::have_complete_line() const
{
    size_t first_line_ending = buffer_.find( CRLF );
    return first_line_ending != std::string::npos;
}

template <class MessageType>
std::string HTTPMessageSequence<MessageType>::InternalBuffer::get_and_pop_line()
{
    size_t first_line_ending = buffer_.find( CRLF );
    assert( first_line_ending != std::string::npos );

    std::string first_line( buffer_.substr( 0, first_line_ending ) );
    pop_bytes( first_line_ending + CRLF.size() );

    return first_line;
}

template <class MessageType>
void HTTPMessageSequence<MessageType>::InternalBuffer::pop_bytes( const size_t num )
{
    assert( buffer_.size() >= num );
    buffer_.replace( 0, num, std::string() );
}

template <class MessageType>
bool HTTPMessageSequence<MessageType>::parsing_step()
{
    switch ( message_in_progress_.state() ) {
    case FIRST_LINE_PENDING:
        /* do we have a complete line? */
        if ( not buffer_.have_complete_line() ) { return false; }

        /* supply status line to request/response initialization routine */
        initialize_new_message();

        message_in_progress_.set_first_line( buffer_.get_and_pop_line() );

        return true;
    case HEADERS_PENDING:
        /* do we have a complete line? */
        if ( not buffer_.have_complete_line() ) { return false; }

        /* is line blank? */
        {
            std::string line( buffer_.get_and_pop_line() );
            if ( line.empty() ) {
                message_in_progress_.done_with_headers();
            } else {
                message_in_progress_.add_header( line );
            }
        }
        return true;

    case BODY_PENDING:
        {
            size_t bytes_read = message_in_progress_.read_in_body( buffer_.str() );
            assert( bytes_read == buffer_.str().size() or message_in_progress_.state() == COMPLETE );
            buffer_.pop_bytes( bytes_read );
        }
        return message_in_progress_.state() == COMPLETE;

    case COMPLETE:
        complete_messages_.emplace( std::move( message_in_progress_ ) );
        message_in_progress_ = MessageType();
        return true;
    }

    assert( false );
    return false;
}

template <class MessageType>
void HTTPMessageSequence<MessageType>::parse( const std::string & buf )
{
    if ( buf.empty() ) { /* EOF */
        message_in_progress_.eof();
    }

    /* append buf to internal buffer */
    buffer_.append( buf );

    /* parse as much as we can */
    while ( parsing_step() ) {}
}

#endif /* HTTP_MESSAGE_SEQUENCE */
