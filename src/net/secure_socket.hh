/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/* run: sudo apt-get install libssl-dev */

#pragma once
#include <memory>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "socket.hh"
#include "util/exception.hh"

/* error category for OpenSSL */
class ssl_error_category : public std::error_category
{
public:
    const char * name( void ) const noexcept override { return "SSL"; }
    std::string message( const int ssl_error ) const noexcept override
    {
        return ERR_error_string( ssl_error, nullptr );
    }
};

class ssl_error : public tagged_error
{
public:
    ssl_error( const std::string & s_attempt,
               const int error_code = ERR_get_error() )
        : tagged_error( ssl_error_category(), s_attempt, error_code )
    {}
};

class SecureSocket : public TCPSocket
{
    friend class SSLContext;

private:
    struct SSL_deleter { void operator()( SSL * x ) const { SSL_free( x ); } };
    typedef std::unique_ptr<SSL, SSL_deleter> SSL_handle;
    SSL_handle ssl_;

    SecureSocket( TCPSocket && sock, SSL * ssl );

public:
    void connect( void );
    void accept( const bool register_as_write = false );

    std::string read( const bool register_as_write = false );
    void write( const std::string & message, const bool register_as_read = false );
    int get_error( const int return_value );
};

class SSLContext
{
private:
    struct CTX_deleter { void operator()( SSL_CTX * x ) const { SSL_CTX_free( x ); } };
    typedef std::unique_ptr<SSL_CTX, CTX_deleter> CTX_handle;
    CTX_handle ctx_;

public:
    SSLContext();

    SecureSocket new_secure_socket( TCPSocket && sock );
};
