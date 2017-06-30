/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <vector>
#include <string>

#include "model-base.hh"
#include "thunk.hh"

static const std::string GCC_COMPILER = ".gg/exe/bin/x86_64-linux-musl-gcc";
static const std::string AS = ".gg/exe/bin/as";
static const std::string CC1 = ".gg/exe/bin/cc1";

class ModelCompile : public ModelBase
{
protected:
  std::vector<gg::thunk::InFile> get_infiles()
  {
    return { srcfile, GCC_COMPILER, CC1 };
  }

public:
  ModelCompile( int argc, char **argv ) : ModelBase( argc, argv ) {}
  ModelCompile( const std::vector<std::string> & args ) : ModelBase( args ) {}

  ~ModelCompile() {}
};

class ModelAssemble : public ModelBase
{
protected:
  std::vector<gg::thunk::InFile> get_infiles()
  {
    return { srcfile, GCC_COMPILER, AS };
  }

public:
  ModelAssemble( int argc, char **argv ) : ModelBase( argc, argv ) {}
  ModelAssemble( const std::vector<std::string> & args ) : ModelBase( args ) {}

  ~ModelAssemble() {}
};
