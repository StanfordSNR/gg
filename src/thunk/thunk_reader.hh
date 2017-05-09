/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#pragma once

#include <string>
#include <vector>

#include "gg.pb.h"
#include "optional.hh"
#include "thunk.hh"

class ThunkReader
{
private:
  ThunkReader();

public:
  /* returns an empty optional if the file is not a valid Thunk */
  static Optional<gg::thunk::Thunk> read_thunk( const std::string & filename );
};
