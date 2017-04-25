/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#pragma once

#include <string>
#include <vector>

#include "thunk.hh"

class ThunkWriter
{
private:
  ThunkWriter();

public:
  static bool write_thunk( Thunk thunk );

  static Thunk read_thunk( std::string filename );
};
