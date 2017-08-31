/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/* run: sudo apt-get install libssl-dev */

#pragma once
#include <memory>
#include <openssl/ssl.h>

#include "socket.hh"

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
    void accept( void );
    void close( void );
    void set_hostname(const std::string &hostname); 

    std::string read( void );
    void write( const std::string & message );
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
