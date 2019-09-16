/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <string>
#include <cstring>
#include <memory>
#include <cassert>
#include <functional>
#include <netdb.h>

#include "address.hh"
#include "strict_conversions.hh"
#include "util/util.hh"
#include "util/exception.hh"

using namespace std;

template <typename T> void zero( T & x ) { memset( &x, 0, sizeof( x ) ); }

/* constructors */

Address::Address()
    : Address( "0", 0 )
{}

Address::Address( const raw & addr, const size_t size )
    : Address( addr.as_sockaddr, size )
{}

Address::Address( const sockaddr & addr, const size_t size )
    : size_( size ),
      addr_()
{
    /* make sure proposed sockaddr can fit */
    if ( size > sizeof( addr_ ) ) {
        throw runtime_error( "invalid sockaddr size" );
    }

    memcpy( &addr_, &addr, size_ );
}

Address::Address( const sockaddr_in & addr )
    : size_( sizeof( sockaddr_in ) ),
      addr_()
{
    assert( size_ <= sizeof( addr_ ) );

    memcpy( &addr_, &addr, size_ );
}

/* error category for getaddrinfo and getnameinfo */
class gai_error_category : public error_category
{
public:
    const char * name( void ) const noexcept override { return "gai_error_category"; }
    string message( const int return_value ) const noexcept override
    {
        return gai_strerror( return_value );
    }
};

/* private constructor given ip/host, service/port, and optional hints */
Address::Address( const string & node, const string & service, const addrinfo & hints )
    : size_(),
      addr_()
{
    /* prepare for the answer */
    addrinfo *resolved_address;

    /* look up the name or names */
    const int gai_ret = getaddrinfo( node.c_str(), service.c_str(), &hints, &resolved_address );
    if ( gai_ret ) {
        string explanation = "getaddrinfo(" + node + ":" + service;
        if ( hints.ai_flags & (AI_NUMERICHOST | AI_NUMERICSERV) ) {
            explanation += ", numeric";
        }
        explanation += ")";
        throw tagged_error( gai_error_category(), explanation, gai_ret );
    }

    /* if success, should always have at least one entry */
    if ( not resolved_address ) {
        throw runtime_error( "getaddrinfo returned successfully but with no results" );
    }

    /* put resolved_address in a wrapper so it will get freed if we have to throw an exception */
    unique_ptr<addrinfo, function<void(addrinfo*)>> wrapped_address
        { resolved_address, []( addrinfo * x ) { freeaddrinfo( x ); } };

    /* assign to our private members (making sure size fits) */
    *this = Address( *wrapped_address->ai_addr, wrapped_address->ai_addrlen );
}

/* construct by resolving host name and service name */
Address::Address( const std::string & hostname, const std::string & service )
    : size_(),
      addr_()
{
    addrinfo hints;
    zero( hints );
    hints.ai_family = AF_INET;

    *this = Address( hostname, service, hints );
}

/* construct with numerical IP address and numeral port number */
Address::Address( const std::string & ip, const uint16_t port )
    : size_(),
      addr_()
{
    /* tell getaddrinfo that we don't want to resolve anything */
    addrinfo hints;
    zero( hints );
    hints.ai_family = AF_INET;
    hints.ai_flags = AI_NUMERICHOST | AI_NUMERICSERV;

    *this = Address( ip, ::to_string( port ), hints );
}

/* accessors */

pair<string, uint16_t> Address::ip_port( void ) const
{
    char ip[ NI_MAXHOST ], port[ NI_MAXSERV ];

    const int gni_ret = getnameinfo( &to_sockaddr(),
                                     size_,
                                     ip, sizeof( ip ),
                                     port, sizeof( port ),
                                     NI_NUMERICHOST | NI_NUMERICSERV );
    if ( gni_ret ) {
        throw tagged_error( gai_error_category(), "getnameinfo", gni_ret );
    }

    return make_pair( ip, strict_atoi( port ) );
}

string Address::str( const string port_separator ) const
{
    const auto ip_and_port = ip_port();
    return ip_and_port.first + port_separator + to_string( ip_and_port.second );
}

const sockaddr & Address::to_sockaddr( void ) const
{
    return addr_.as_sockaddr;
}

/* comparisons */
bool Address::operator==( const Address & other ) const
{
    return 0 == memcmp( &addr_, &other.addr_, size_ );
}

bool Address::operator<( const Address & other ) const
{
    return (memcmp( &addr_, &other.addr_, sizeof( addr_ ) ) < 0 );
}
