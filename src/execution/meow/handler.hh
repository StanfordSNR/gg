/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef MEOW_HANDLER_HH
#define MEOW_HANDLER_HH

#include <memory>

#include "execution/connection.hh"
#include "execution/meow/message.hh"

namespace meow {

  void handle_message( const Message & message,
                       const std::shared_ptr<TCPConnection> & connection );

}

#endif /* MEOW_HANDLER_HH */
