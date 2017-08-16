/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "ggpaths.hh"

roost::path get_gg_dir()
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
  } else {
    roost::create_directories( gg_path );
  }

  return roost::canonical( gg_path );
}

roost::path get_blobs_path()
{
  return get_gg_dir();
}

roost::path get_reductions_path()
{
  roost::path reductions_dir = get_gg_dir() / "reductions";

  if ( roost::exists( reductions_dir ) ) {
    if ( not roost::is_directory( reductions_dir ) ) {
      throw std::runtime_error( reductions_dir.string() + " is not a directory" );
    }
  } else {
    roost::create_directories( reductions_dir );
  }

  return reductions_dir;
}

roost::path gg::paths::blobs()
{
  const static roost::path blobs_path = get_blobs_path();
  return blobs_path;
}

roost::path gg::paths::reductions()
{
  const static roost::path reductions_path = get_reductions_path();
  return reductions_path;
}

roost::path gg::paths::blob_path( const std::string & hash )
{
  return blobs() / hash;
}

std::vector<std::string> gg::models::args_to_vector( int argc, char ** argv )
{
  std::vector<std::string> result;

  /* ignores argv[ 0 ] */
  for ( int i = 1; i < argc; i++ ) {
    result.push_back( argv[ i ] );
  }

  return result;
}
