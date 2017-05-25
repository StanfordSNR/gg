#pragma once

#include <string.h>

#include "thunk.hh"

class GGModelBase{

protected:
  const std::string GG_DIR;

  static const std::string GG_DIR_FLAG = "GG_DIR";

  virtual gg::thunk::Function get_function() = 0;

  virtual std::string get_outfile() = 0;

  virtual std::vector<gg::thunk::InFile> get_infiles() = 0;

public:
  GGModelBase();

  gg::thunk::Thunk build_thunk();

  void write_thunk();
};