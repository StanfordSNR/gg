/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#pragma once

#include <string.h>

#include "thunk.hh"
#include "gg-model-base.hh"

class GGModelAssemble : public GGModelBase
{
protected:
  std::vector<gg::thunk::InFile> get_infiles();

public:
  GGModelAssemble(int, char**);
  ~GGModelAssemble();
};
