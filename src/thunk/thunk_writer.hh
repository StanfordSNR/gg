/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#pragma once

#include <string>

#include "thunk/thunk.hh"

class ThunkWriter
{
public:
  static std::string write_thunk( const gg::thunk::Thunk & thunk );
  static std::string serialize_thunk( const gg::thunk::Thunk & thunk );
};
