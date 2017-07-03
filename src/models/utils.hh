/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <string>
#include <boost/filesystem.hpp>

namespace gg {
  namespace models {
    boost::filesystem::path gg_dir()
    {
      char * envar = getenv( "GG_DIR" );

      if ( envar == NULL ) {
        return { ".gg/" };
      }
      else {
        return { envar };
      }
    }
  }
}
