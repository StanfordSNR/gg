/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#pragma once

#include <string>
#include <vector>

#include "thunk.hh"
#include "gg.pb.h"

class ThunkWriter
{
private:
  ThunkWriter();

public:
  static std::string write_thunk( const gg::thunk::Thunk & thunk );
  static void write_thunk( const gg::thunk::Thunk & thunk,
                           const std::string & filename );
};
