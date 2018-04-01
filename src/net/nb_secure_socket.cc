/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "nb_secure_socket.hh"

#include <cassert>

#include "util/poller.hh"

using namespace std;

Poller::Action::Action( NBSecureSocket & s_socket,
                        const PollDirection & s_direction,
                        const CallbackType & s_callback,
                        const std::function<bool(void)> & s_when_interested,
                        const std::function<void(void)> & fderror_callback )
  : fd( s_socket ), direction( s_direction ), callback(), when_interested(),
    fderror_callback( fderror_callback ), active( true )
{
  if ( direction == Out ) { /* write */
    callback =
      [s_callback, &s_socket] ()
      {
        Result retval;

        if ( s_socket.mode() == NBSecureSocket::Mode::connect and not s_socket.connected() ) {
          /* we're not connected yet, so let's continue */
          s_socket.continue_SSL_connect();
        }
        else if ( s_socket.mode() == NBSecureSocket::Mode::accept and not s_socket.accepted() ) {
          /* we've not accepted yet, so let's continue */
          s_socket.continue_SSL_accept();
        }
        else if ( s_socket.state() == NBSecureSocket::State::needs_ssl_write_to_write ) {
          s_socket.continue_SSL_write();
        }
        else if ( s_socket.state() == NBSecureSocket::State::ready ) {
          if ( not s_socket.something_to_write() ) {
            retval = s_callback();
          }

          s_socket.continue_SSL_write();
        }
        else if ( s_socket.state() == NBSecureSocket::State::needs_ssl_write_to_read ) {
          s_socket.continue_SSL_read();
        }
        else {
          throw runtime_error( "unexpected state: " + to_string(static_cast<int>(s_socket.state())) );
        }

        return retval;
      };

    when_interested =
      [s_when_interested, &s_socket]()
      {
        return ( s_socket.state() == NBSecureSocket::State::needs_connect ) or
               ( s_socket.state() == NBSecureSocket::State::needs_ssl_write_to_connect ) or
               ( s_socket.state() == NBSecureSocket::State::needs_ssl_write_to_accept ) or
               ( s_socket.state() == NBSecureSocket::State::needs_ssl_write_to_write ) or
               ( s_socket.state() == NBSecureSocket::State::needs_ssl_write_to_read ) or
               ( s_socket.state() == NBSecureSocket::State::ready and s_when_interested() );
      };

  }
  else /* direction == In */ { /* read */
    callback =
      [s_callback, &s_socket] ()
      {
        if ( s_socket.mode() == NBSecureSocket::Mode::connect and not s_socket.connected() ) {
          /* we're not connected yet, so let's continue */
          s_socket.continue_SSL_connect();
        }
        else if ( s_socket.mode() == NBSecureSocket::Mode::accept and not s_socket.accepted() ) {
          /* we've not accepted yet, so let's continue */
          s_socket.continue_SSL_accept();
        }
        else if ( s_socket.state() == NBSecureSocket::State::needs_ssl_read_to_write ) {
          s_socket.continue_SSL_write();
        }
        else if ( s_socket.state() == NBSecureSocket::State::needs_ssl_read_to_read or
                  s_socket.state() == NBSecureSocket::State::ready ) {
          s_socket.continue_SSL_read();
        }
        else {
          throw runtime_error( "unexpected state" );
        }

        if ( s_socket.something_to_read() ) {
          return s_callback();
        }

        return Result {};
      };

    when_interested =
      [s_when_interested, &s_socket]()
      {
        return ( s_socket.state() == NBSecureSocket::State::needs_connect ) or
               ( s_socket.state() == NBSecureSocket::State::needs_accept ) or
               ( s_socket.state() == NBSecureSocket::State::needs_ssl_read_to_connect ) or
               ( s_socket.state() == NBSecureSocket::State::needs_ssl_read_to_accept ) or
               ( s_socket.state() == NBSecureSocket::State::needs_ssl_read_to_write ) or
               ( s_socket.state() == NBSecureSocket::State::needs_ssl_read_to_read ) or
               ( s_socket.state() == NBSecureSocket::State::ready and s_when_interested() );
      };
  }
}

void NBSecureSocket::connect()
{
  mode_ = Mode::connect;
  state_ = State::needs_connect;
}

void NBSecureSocket::accept()
{
  mode_ = Mode::accept;
  state_ = State::needs_accept;
}

void NBSecureSocket::continue_SSL_connect()
{
  if ( state_ == State::needs_connect ) {
    verify_no_errors();
  }

  if ( state_ == State::needs_connect or
       state_ == State::needs_ssl_write_to_connect or
       state_ == State::needs_ssl_read_to_connect )
  {
    try {
      SecureSocket::connect();
    }
    catch ( const ssl_error & s ) {
      switch ( s.error_code() ) {
      case SSL_ERROR_WANT_READ:
        state_ = State::needs_ssl_read_to_connect;
        break;

      case SSL_ERROR_WANT_WRITE:
        state_ = State::needs_ssl_write_to_connect;
        break;

      default:
        throw;
      }

      return;
    }

    state_ = State::ready;
    return;
  }

  assert( ready() );
  throw runtime_error( "session already connected" );
}

void NBSecureSocket::continue_SSL_accept()
{
  if ( state_ == State::needs_accept ) {
    verify_no_errors();
  }

  if ( state_ == State::needs_accept or
       state_ == State::needs_ssl_write_to_accept or
       state_ == State::needs_ssl_read_to_accept )
  {
    try {
      SecureSocket::accept( state_ == State::needs_ssl_write_to_accept );
    }
    catch ( const ssl_error & s ) {
      switch ( s.error_code() ) {
      case SSL_ERROR_WANT_READ:
        state_ = State::needs_ssl_read_to_accept;
        break;

      case SSL_ERROR_WANT_WRITE:
        state_ = State::needs_ssl_write_to_accept;
        break;

      default:
        throw;
      }

      return;
    }

    state_ = State::ready;
    return;
  }

  assert( ready() );
  throw runtime_error( "session already connected" );
}

void NBSecureSocket::continue_SSL_write()
{
  try {
    SecureSocket::write( write_buffer_.size() ? write_buffer_.front() : string(),
                         state_ == State::needs_ssl_read_to_write );
  }
  catch ( ssl_error & s ) {
    switch ( s.error_code() ) {
    case SSL_ERROR_WANT_READ:
      state_ = State::needs_ssl_read_to_write;
      break;

    case SSL_ERROR_WANT_WRITE:
      state_ = State::needs_ssl_write_to_write;
      break;

    default:
      throw;
    }

    return;
  }

  write_buffer_.pop_front();
  state_ = State::ready;
}

void NBSecureSocket::continue_SSL_read()
{
  try {
    read_buffer_ += SecureSocket::read( state_ == State::needs_ssl_write_to_read );
  }
  catch ( ssl_error & s ) {
    switch ( s.error_code() ) {
    case SSL_ERROR_WANT_READ:
      state_ = State::needs_ssl_read_to_read;
      break;

    case SSL_ERROR_WANT_WRITE:
      state_ = State::needs_ssl_write_to_read;
      break;

    default:
      throw;
    }

    return;
  }

  state_ = State::ready;
}

string NBSecureSocket::ezread()
{
  string buffer { move( read_buffer_ ) };
  read_buffer_ = string {};
  return buffer;
}

unsigned int NBSecureSocket::buffer_bytes() const
{
  unsigned int total_bytes = 0;

  for ( const auto & buffer : write_buffer_ ) {
    total_bytes += buffer.size();
  }

  return total_bytes;
}
