/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "connection_context.hh"

#include <stdexcept>

using namespace std;

void ConnectionContext::continue_SSL_connect()
{
  if ( state == State::needs_connect ) {
    socket.verify_no_errors();
    /* TCP successfully connected, so start SSL session */

    state = State::needs_ssl_write_to_connect;
  }

  if ( state == State::needs_ssl_write_to_connect
       or state == State::needs_ssl_read_to_connect ) {
    try {
      socket.connect();
    } catch ( const ssl_error & s ) {
      /* is it a WANT_READ or WANT_WRITE? */
      switch ( s.error_code() ) {
      case SSL_ERROR_WANT_READ:
        state = State::needs_ssl_read_to_connect;
        break;

      case SSL_ERROR_WANT_WRITE:
        state = State::needs_ssl_write_to_connect;
        break;

      default:
        throw;
      }

      return;
    }

    state = State::ready;
    return;
  }

  assert( ready() );
  throw runtime_error( "session already connected" );
}

void ConnectionContext::continue_SSL_write()
{
  try {
    socket.write( request_str, state == State::needs_ssl_read_to_write );
  }
  catch ( ssl_error & s ) {
    switch ( s.error_code() ) {
    case SSL_ERROR_WANT_READ:
      state = State::needs_ssl_read_to_write;
      break;

    case SSL_ERROR_WANT_WRITE:
      state = State::needs_ssl_write_to_write;
      break;

    default:
      throw;
    }

    return;
  }

  something_to_write = false;
  state = State::ready;
}

void ConnectionContext::continue_SSL_read()
{
  try {
    responses.parse( socket.read( state == State::needs_ssl_write_to_read ) );
  }
  catch ( ssl_error & s ) {
    switch ( s.error_code() ) {
    case SSL_ERROR_WANT_READ:
      state = State::needs_ssl_read_to_read;
      break;

    case SSL_ERROR_WANT_WRITE:
      state = State::needs_ssl_write_to_read;
      break;

    default:
      throw;
    }

    return;
  }

  state = State::ready;
}
