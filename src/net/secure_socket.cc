/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <cassert>
#include <vector>
#include <thread>
#include <mutex>

#include "secure_socket.hh"
#include "util/exception.hh"

using namespace std;

class OpenSSL
{
private:
    vector<mutex> locks_;

    static void locking_function( int mode, int n, const char *, int )
    {
        if ( mode & CRYPTO_LOCK ) {
            OpenSSL::global_context().locks_.at( n ).lock();
        } else {
            OpenSSL::global_context().locks_.at( n ).unlock();
        }
    }

    static unsigned long id_function( void )
    {
        return pthread_self();
    }

public:
    OpenSSL()
        : locks_( CRYPTO_num_locks() )
    {
        /* SSL initialization: Needs to be done exactly once */
        /* load algorithms/ciphers */
        SSL_library_init();
        OpenSSL_add_all_algorithms();

        /* load error messages */
        SSL_load_error_strings();

        /* set thread-safe callbacks */
        CRYPTO_set_locking_callback( locking_function );
        CRYPTO_set_id_callback( id_function );
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
    SSL_CTX * ret = SSL_CTX_new( SSLv23_client_method() );
    if ( not ret ) {
        throw ssl_error( "SSL_CTL_new" );
    }
    return ret;
}

SSLContext::SSLContext()
    : ctx_( initialize_new_context() )
{}

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
    SSL_set_mode( ssl_.get(), SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER );
}

SecureSocket SSLContext::new_secure_socket( TCPSocket && sock )
{
    return SecureSocket( move( sock ),
                         SSL_new( ctx_.get() ) );
}

void SecureSocket::connect( void )
{
    ERR_clear_error();
    int retval = SSL_connect( ssl_.get() );

    if ( not retval ) {
        throw ssl_error( "SSL_connect", SSL_get_error( ssl_.get(), retval ) );
    }

    register_read();
    register_write();
}

void SecureSocket::accept( const bool register_as_write )
{
    ERR_clear_error();
    const auto ret = SSL_accept( ssl_.get() );

    if ( ret == 1 ) {
        register_read();
        return;
    } else {
        int error_return = SSL_get_error( ssl_.get(), ret );

        if ( error_return == SSL_ERROR_WANT_WRITE or error_return == SSL_ERROR_WANT_READ ) {
          register_service( register_as_write );
        }

        throw ssl_error( "SSL_accept", error_return );
    }
}

string SecureSocket::read( const bool register_as_write )
{
    /* SSL record max size is 16kB */
    const size_t SSL_max_record_length = 16384;

    char buffer[ SSL_max_record_length ];

    ERR_clear_error();
    ssize_t bytes_read = SSL_read( ssl_.get(), buffer, SSL_max_record_length );

    /* Make sure that we really are reading from the underlying fd */
    assert( 0 == SSL_pending( ssl_.get() ) );
    int error_return = SSL_get_error( ssl_.get(), bytes_read );

    if ( bytes_read == 0 ) {
        if ( SSL_ERROR_ZERO_RETURN == error_return ) { /* Clean SSL close */
            set_eof();
        } else if ( SSL_ERROR_SYSCALL == error_return ) { /* Underlying TCP connection close */
            /* Verify error queue is empty so we can conclude it is EOF */
            assert( ERR_get_error() == 0 );
            set_eof();
        }
        register_service( register_as_write );
        return string(); /* EOF */
    } else if ( bytes_read < 0 ) {
        if ( error_return == SSL_ERROR_WANT_WRITE or error_return == SSL_ERROR_WANT_READ ) {
          register_service( register_as_write );
        }

        throw ssl_error( "SSL_read", error_return );
    } else {
        /* success */
        register_service( register_as_write );
        return string( buffer, bytes_read );
    }
}

void SecureSocket::write( const string & message, const bool register_as_read )
{
    /* SSL_write returns with success if complete contents of message are written */
    ERR_clear_error();
    ssize_t bytes_written = SSL_write( ssl_.get(), message.data(), message.length() );

    if ( bytes_written <= 0 ) {
        int error_return = SSL_get_error( ssl_.get(), bytes_written );

        if ( error_return == SSL_ERROR_WANT_WRITE or error_return == SSL_ERROR_WANT_READ ) {
          register_service( not register_as_read );
        }

        throw ssl_error( "SSL_write", error_return );
    }

    register_service( not register_as_read );
}

int SecureSocket::get_error( const int return_value )
{
    return SSL_get_error( ssl_.get(), return_value );
}
