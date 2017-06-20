/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <vector>

#include "gg-model-base.hh"
#include "thunk.hh"

static const std::string GCC_COMPILER = ".gg/exe/bin/x86_64-linux-musl-gcc";
static const std::string AS = ".gg/exe/bin/as";
static const std::string CC1 = ".gg/exe/bin/cc1";

class GGModelCompile : public GGModelBase
{
protected:
  std::vector<gg::thunk::InFile> get_infiles();

public:
  GGModelCompile( int argc, char **argv )
    : GGModelBase( argc, argv ) {}

  ~GGModelCompile() {}
};

class GGModelAssemble : public GGModelBase
{
protected:
  std::vector<gg::thunk::InFile> get_infiles();

public:
  GGModelAssemble( int argc, char **argv )
    : GGModelBase( argc, argv ) {}

  ~GGModelAssemble() {}
};
