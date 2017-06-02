/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "gg-model-base.hh"

#include <iostream>
#include <unistd.h>

using namespace std;
using namespace gg::thunk;

static const string GCC_COMPILER = ".gg/exe/bin/x86_64-linux-musl-gcc";
static const string CC1 = ".gg/exe/bin/cc1";

class GGModelCompile : public GGModelBase
{
protected:
  vector<InFile> get_infiles()
  {
    vector<InFile> infiles { srcfile, GCC_COMPILER, CC1 };
    return infiles;
  }

public:
  GGModelCompile( int argc, char **argv )
    : GGModelBase(argc, argv) {}

  ~GGModelCompile() {}
};

int main( int argc, char ** argv )
{
  GGModelCompile model { argc, argv };
  model.write_thunk();
}
