/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <boost/filesystem.hpp>

#include "thunk.hh"
#include "utils.hh"

namespace fs = boost::filesystem;

int main( int, char *[] )
{
  fs::path gg_dir = gg::models::create_gg_dir();
  return 0;
}
