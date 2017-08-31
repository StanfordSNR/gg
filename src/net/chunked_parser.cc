/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <cassert>

#include "strict_conversions.hh"
#include "chunked_parser.hh"

using namespace std;

/* Take a chunk header and parse it assuming no folding */
uint32_t ChunkedBodyParser::get_chunk_size( const string & chunk_hdr ) const
{
    /* Check that the chunk header ends with a CRLF */
    assert( chunk_hdr.substr( chunk_hdr.length() - 2, 2 ) == "\r\n" );

    /* If there are chunk extensions, ';' terminates chunk size */
    auto pos = chunk_hdr.find( ";" );

    /* There are no ';'s, and hence no chunk externsions, CRLF terminates chunk size */
    if ( pos == string::npos ) {
        pos = chunk_hdr.find( "\r\n" );
    }

    /* Can't be npos even now */
    assert( pos != string::npos );

    /* Parse hex string, after removing trailing spaces (RFC 2616 Section 2.1) */
    auto hex_string = chunk_hdr.substr( 0, pos );
    auto space_loc = hex_string.find( " " );
    if ( space_loc != string::npos ) hex_string.erase( space_loc, hex_string.length() - space_loc );
    return strict_atoi( hex_string, 16 );
}

string::size_type ChunkedBodyParser::read( const std::string & input_buffer )
{
    parser_buffer_ += input_buffer;

    while ( !parser_buffer_.empty() ) {
        switch (state_) {
        case CHUNK_HDR: {
            auto it = parser_buffer_.find( "\r\n" );
            if (it != string::npos) {
                /* if you have CRLF, get chunk size & transition to CHUNK/TRAILER */
                current_chunk_size_ = get_chunk_size( parser_buffer_.substr( 0, it + 2 ) );

                /* Transition appropriately */
                state_ = ( current_chunk_size_ == 0 ) ? TRAILER : CHUNK;

                /* shrink parser_buffer_ */
                parsed_so_far_ += ( it + 2 );
                parser_buffer_ = parser_buffer_.substr( it + 2 );
                break;
            } else {
                /* if you haven't seen a CRLF so far, do nothing */
                acked_so_far_ += input_buffer.length();
                return string::npos;
            }
        }

        case CHUNK: {
            if ( parser_buffer_.length() >= current_chunk_size_ + 2 ) {
                /* accumulated enough bytes, check CRLF at the end of the chunk */
                assert( parser_buffer_.substr( current_chunk_size_, 2 ) == "\r\n" );

                /* Transition to next state */
                state_ = CHUNK_HDR;

                /* shrink parser_buffer_ */
                parsed_so_far_ += current_chunk_size_ + 2;
                parser_buffer_ = parser_buffer_.substr( current_chunk_size_ + 2 );
                break;
            } else {
                /* Haven't seen enough bytes so far, do nothing */
                acked_so_far_ += input_buffer.length();
                return string::npos;
            }
        }

        case TRAILER: {
            if ( trailers_enabled_) {
                /* We need two consecutive CRLFs */
                return compute_ack_size( parser_buffer_,
                                         "\r\n\r\n",
                                         input_buffer.length() );
            } else {
                /* We need only one CRLF now */
                return compute_ack_size( parser_buffer_,
                                         "\r\n",
                                         input_buffer.length() );
            }
        }

        default: {
            assert( false );
            return false;
        }
        }
    }
    acked_so_far_ += input_buffer.length();
    return string::npos;
}

/*
   Computes the acknowledgement from the BodyParser to its caller,
   telling it how much of the current input_buffer has been
   successfully parsed.
*/
string::size_type ChunkedBodyParser::compute_ack_size( const string & haystack,
                                                       const string & needle,
                                                       const string::size_type input_size )
{
    auto loc = haystack.find( needle );
    if ( loc != string::npos ) {
        /* Found it, eat up the whole buffer */
        parsed_so_far_ += loc + needle.length();
        assert( parsed_so_far_ > acked_so_far_ );
        return ( parsed_so_far_ - acked_so_far_ );
    } else {
        /* Find unacknowledged buffer so far, ack it, and be done */
        acked_so_far_ += input_size;
        return loc;
    }
}
