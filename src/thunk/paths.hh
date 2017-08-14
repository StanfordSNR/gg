/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef PATHS_HH
#define PATHS_HH

#include <string>
#include <stdexcept>
#include <vector>

#include "path.hh"

namespace gg {
  namespace paths {
    roost::path blobs();
    roost::path reductions();

    roost::path blob_path( const std::string & hash );
  }

  namespace models {
    std::vector<std::string> args_to_vector( int argc, char ** argv );
  }
}

#endif /* PATHS_HH */
