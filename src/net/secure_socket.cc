/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <cassert>
#include <vector>
#include <openssl/err.h>

#include "secure_socket.hh"
#include "exception.hh"

using namespace std;

// These are the ciphers we're willing to use. This is a pretty restrictive list.
static const char SSL_CIPHERS_[] = "ECDHE-RSA-AES256-GCM-SHA384:ECDHE-RSA-AES256-SHA384:ECDHE-RSA-AES128-GCM-SHA256:ECDHE-RSA-AES128-SHA256:ECDHE-RSA-AES256-SHA:HIGH:!aNULL:!eNULL:!EXP:!LOW:!MEDIUM:!MD5:!RC4:!DES:!3DES";

/* error category for OpenSSL */
class ssl_error_category : public error_category
{
public:
    const char * name( void ) const noexcept override { return "SSL"; }
    string message( const int ssl_error ) const noexcept override
    {
        return ERR_error_string( ssl_error, nullptr );
    }
};

class ssl_error : public tagged_error
{
public:
    ssl_error( const string & s_attempt,
               const int error_code = ERR_get_error() )
        : tagged_error( ssl_error_category(), s_attempt, error_code )
    {}
};

class OpenSSL
{
public:
    OpenSSL()
    {
        /* SSL initialization: Needs to be done exactly once */
        /* load algorithms/ciphers */
        SSL_library_init();
        OpenSSL_add_all_algorithms();

        /* load error messages */
        SSL_load_error_strings();
    }

    static OpenSSL & global_context( void )
    {
        static OpenSSL os;
        return os;
    }
};

SSL_CTX * initialize_new_context()
{
    OpenSSL::global_context();

#if OPENSSL_VERSION_NUMBER < 0x10100000L
    SSL_CTX * ret = SSL_CTX_new(TLSv1_2_client_method());
#else
    SSL_CTX * ret = SSL_CTX_new(TLS_client_method());
#endif
    if ( not ret ) {
        throw ssl_error( "SSL_CTL_new" );
    }

    // set some pretty conservative options
    // XXX: whatever happened to SSL_OP_NO_COMPRESSION ???
    SSL_CTX_set_options(ret, SSL_OP_SINGLE_DH_USE);
    SSL_CTX_set_options(ret, SSL_OP_NO_SSLv2);
    SSL_CTX_set_options(ret, SSL_OP_NO_SSLv3);
    SSL_CTX_set_options(ret, SSL_OP_NO_TLSv1);
    SSL_CTX_set_options(ret, SSL_OP_NO_TLSv1_1);
    SSL_CTX_clear_options(ret, SSL_OP_NO_TLSv1_2);
    if (SSL_CTX_set_cipher_list(ret, (const char *)SSL_CIPHERS_) != 1) {
        throw ssl_error( "SSL_CTX_set_cipher_list" );
    }
    // XXX: add a verification callback here
    SSL_CTX_set_verify(ret, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, NULL);
    SSL_CTX_set_verify_depth(ret, 9);
    if (SSL_CTX_set_default_verify_paths(ret) != 1) {
        throw ssl_error( "SSL_CTX_set_default_verify_paths" );
    }

    return ret;
}

SSLContext::SSLContext()
    : ctx_( initialize_new_context() ) {}

SecureSocket::SecureSocket( TCPSocket && sock, SSL * ssl )
    : TCPSocket( move( sock ) ),
      ssl_( ssl )
{
    if ( not ssl_ ) {
        throw runtime_error( "SecureSocket: constructor must be passed valid SSL structure" );
    }

    if ( not SSL_set_fd( ssl_.get(), fd_num() ) ) {
        throw ssl_error( "SSL_set_fd" );
    }

    /* enable read/write to return only after handshake/renegotiation and successful completion */
    SSL_set_mode( ssl_.get(), SSL_MODE_AUTO_RETRY );
}

SecureSocket SSLContext::new_secure_socket( TCPSocket && sock )
{
    return SecureSocket( move( sock ),
                         SSL_new( ctx_.get() ) );
}

void SecureSocket::connect( void )
{
    if ( SSL_connect( ssl_.get() ) != 1 ) {
        throw ssl_error( "SSL_connect" );
    }
}

void SecureSocket::accept( void )
{
    const auto ret = SSL_accept( ssl_.get() );
    if ( ret == 1 ) {
        return;
    } else {
        throw ssl_error( "SSL_accept" );
    }

    register_read();
}

string SecureSocket::read( void )
{
    /* SSL record max size is 16kB */
    const size_t SSL_max_record_length = 16384;

    char buffer[ SSL_max_record_length ];

    ssize_t bytes_read = SSL_read( ssl_.get(), buffer, SSL_max_record_length );

    /* Make sure that we really are reading from the underlying fd */
    assert( 0 == SSL_pending( ssl_.get() ) );

    if ( bytes_read == 0 ) {
        int error_return = SSL_get_error( ssl_.get(), bytes_read );
        if ( SSL_ERROR_ZERO_RETURN == error_return ) { /* Clean SSL close */
            set_eof();
        } else if ( SSL_ERROR_SYSCALL == error_return ) { /* Underlying TCP connection close */
            /* Verify error queue is empty so we can conclude it is EOF */
            assert( ERR_get_error() == 0 );
            set_eof();
        }
        register_read();
        return string(); /* EOF */
    } else if ( bytes_read < 0 ) {
        throw ssl_error( "SSL_read" );
    } else {
        /* success */
        register_read();
        return string( buffer, bytes_read );
    }
}

void SecureSocket::write(const string & message )
{
    /* SSL_write returns with success if complete contents of message are written */
    ssize_t bytes_written = SSL_write( ssl_.get(), message.data(), message.length() );

    if ( bytes_written < 0 ) {
        throw ssl_error( "SSL_write" );
    }

    register_write();
}

void SecureSocket::close(void) {
    // XXX shouldn't ignore this return value
    SSL_shutdown( ssl_.get() );
    TCPSocket::close();
}

void SecureSocket::set_hostname(const string &hostname) {
    // XXX shouldn't ignore this return value
    SSL_set_tlsext_host_name(ssl_.get(), (char *)hostname.c_str());
}
