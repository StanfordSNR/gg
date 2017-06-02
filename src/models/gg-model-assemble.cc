/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "gg-model-base.hh"

#include <iostream>
#include <unistd.h>

using namespace std;
using namespace gg::thunk;

static const string GCC_COMPILER = ".gg/exe/bin/x86_64-linux-musl-gcc";
static const string AS = ".gg/exe/bin/as";

class GGModelAssemble : public GGModelBase
{
protected:
  vector<InFile> get_infiles()
  {
    vector<InFile> infiles { srcfile, GCC_COMPILER, AS };
    return infiles;
  }

public:
  GGModelAssemble( int argc, char **argv )
    : GGModelBase(argc, argv) {}

  ~GGModelAssemble() {}
};

int main( int argc, char ** argv )
{
  GGModelAssemble model { argc, argv };
  model.write_thunk();
}
