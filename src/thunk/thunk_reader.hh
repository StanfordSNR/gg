/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#pragma once

#include <string>
#include <vector>

#include "thunk.hh"
#include "gg.pb.h"

class ThunkReader
{
private:
  ThunkReader();

public:
  static gg::thunk::Thunk read_thunk( const std::string & filename );
};
