/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <stdexcept>

#include "path.hh"

using namespace std;

namespace roost {
  path::path( const std::string & pathn )
    : path_( pathn )
  {}

  path path::lexically_normal() const
  {
    return boost_path().lexically_normal().string();
  }

  const string & path::string() const
  {
    return path_;
  }

  boost::filesystem::path path::boost_path() const /* XXX */
  {
    return boost::filesystem::path( path_ );
  }
  
  bool exists( const path & pathn )
  {
    return boost::filesystem::exists( pathn.boost_path() );
  }

  size_t file_size( const path & pathn )
  {
    return boost::filesystem::file_size( pathn.boost_path() );
  }

  path absolute( const path & pathn )
  {
    return boost::filesystem::absolute( pathn.boost_path() ).string();
  }

  path canonical( const path & pathn )
  {
    return boost::filesystem::canonical( pathn.boost_path() ).string();
  }
  
  void copy_file( const path & src, const path & dst )
  {
    return boost::filesystem::copy_file( src.boost_path(),
					 dst.boost_path(),
					 boost::filesystem::copy_option::overwrite_if_exists );
  }

  path operator/( const path & prefix, const path & suffix )
  {
    return (prefix.boost_path() / suffix.boost_path()).string();
  }

  void create_directories( const path & pathn )
  {
    if ( not boost::filesystem::create_directories( pathn.boost_path() ) ) {
      throw runtime_error( "could not create directory: " + pathn.string() );
    }
  }

  bool is_directory( const path & pathn )
  {
    return boost::filesystem::is_directory( pathn.boost_path() );
  }
}
