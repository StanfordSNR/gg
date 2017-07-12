/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <string>
#include <boost/filesystem.hpp>

namespace gg {
  namespace models {
    boost::filesystem::path gg_dir()
    {
      char * envar = getenv( "GG_DIR" );
      boost::filesystem::path gg_path;

      if ( envar == NULL ) {
        gg_path  = ".gg";
      }
      else {
        gg_path = envar;
      }

      return boost::filesystem::absolute( gg_path );
    }
  }
}
