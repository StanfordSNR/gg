/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <string>
#include <stdexcept>
#include <vector>
#include <boost/filesystem.hpp>


namespace gg {
  namespace models {
    boost::filesystem::path create_gg_dir()
    {
      char * envar = getenv( "GG_DIR" );
      boost::filesystem::path gg_path;

      if ( envar == NULL ) {
        gg_path  = ".gg";
      }
      else {
        gg_path = envar;
      }

      /* create the gg directory, if it doesn't exist */
      if ( not boost::filesystem::create_directories( gg_path ) ) {
        if ( not boost::filesystem::is_directory( gg_path ) ) {
          throw std::runtime_error( "cannot create gg dir" );
        }
      }

      return boost::filesystem::absolute( gg_path );
    }

    std::vector<std::string> args_to_vector( int argc, char ** argv )
    {
      std::vector<std::string> result;

      /* ignores argv[ 0 ] */
      for ( int i = 1; i < argc; i++ ) {
        result.push_back( argv[ i ] );
      }

      return result;
    }
  }
}
