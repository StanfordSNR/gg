/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef MEOW_UTIL_HHs
#define MEOW_UTIL_HHs

#include <string>
#include <vector>
#include <memory>

#include "execution/connection.hh"
#include "execution/meow/message.hh"
#include "thunk/thunk.hh"

namespace meow {

  std::string handle_put_message( const Message & message );
  Message create_put_message( const std::string & hash );
  Message create_execute_message( const gg::thunk::Thunk & thunk,
                                  const std::vector<std::pair<std::string, uint32_t>> & alt_objects = {} );

}

#endif /* MEOW_UTIL_HHs */
