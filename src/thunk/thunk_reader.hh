/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#pragma once

#include <string>
#include <vector>

#include "gg.pb.h"
#include "serialization.hh"
#include "thunk.hh"

class ThunkReader
{
private:
  ProtobufDeserializer deserializer_;
  bool is_thunk_;

public:
  ThunkReader( const std::string & filename );

  bool is_thunk() const { return is_thunk_; }
  gg::thunk::Thunk read_thunk();
};
