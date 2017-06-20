/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "model-gcc.hh"

#include <iostream>
#include <unistd.h>

using namespace std;
using namespace gg::thunk;

vector<InFile> GGModelAssemble::get_infiles()
{
  vector<InFile> infiles { srcfile, GCC_COMPILER, AS };
  return infiles;
}

int main( int argc, char ** argv )
{
  GGModelAssemble model { argc, argv };
  model.write_thunk();
}
