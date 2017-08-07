/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <string>
#include <stdexcept>
#include <vector>

#include "path.hh"

namespace gg {
  namespace models {
    roost::path get_gg_dir( const bool create = true )
    {
      const char * envar = getenv( "GG_DIR" );

      if ( envar == NULL ) {
        throw std::runtime_error( "GG_DIR environment variable not set" );
      }

      roost::path gg_path { std::string( envar ) };

      if ( roost::exists( gg_path ) ) {
        if ( not roost::is_directory( gg_path ) ) {
          throw std::runtime_error( gg_path.string() + " is not a directory" );
        }
      } else if ( create ) { /* does not exist */
        roost::create_directories( gg_path );
      }

      return roost::canonical( gg_path );
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
