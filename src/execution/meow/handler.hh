/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef MEOW_HANDLER_HH
#define MEOW_HANDLER_HH

#include "meow/message.hh"

namespace meow {

  void handle_put( const Message & message );
  Message handle_get( const Message & message );
  void handle_execute( const Message & message );

}

#endif /* MEOW_HANDLER_HH */
