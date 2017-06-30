/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "model-gcc.hh"

#include <iostream>
#include <unistd.h>

using namespace std;
using namespace gg::thunk;

int main( int argc, char ** argv )
{
  ModelCompile model { argc, argv };
  model.write_thunk();
}
