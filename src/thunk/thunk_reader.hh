/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#pragma once

#include <string>
#include <vector>

#include "protobufs/gg.pb.h"
#include "thunk/thunk.hh"
#include "util/serialization.hh"
#include "util/path.hh"

class ThunkReader
{
private:
  ThunkReader();

public:
  static bool is_thunk( const roost::path & path );
  static gg::thunk::Thunk read( const roost::path & path, const std::string & hash = {} );
};
