/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <stdexcept>
#include <unistd.h>

#include "path.hh"

using namespace std;

namespace roost {
  path::path( const std::string & pathn )
    : path_( pathn )
  {}

  path path::lexically_normal() const
  {
    throw runtime_error( "unimplimented" );
  }

  const string & path::string() const
  {
    return path_;
  }

  bool exists( const path & pathn )
  {
    return not access( pathn.string().c_str(), F_OK );
  }

  size_t file_size( const path & pathn )
  {
    return boost::filesystem::file_size( pathn.string() );
  }

  path absolute( const path & pathn )
  {
    return boost::filesystem::absolute( pathn.string() ).string();
  }

  path canonical( const path & pathn )
  {
    return boost::filesystem::canonical( pathn.string() ).string();
  }
  
  void copy_file( const path & src, const path & dst )
  {
    return boost::filesystem::copy_file( src.string(),
					 dst.string(),
					 boost::filesystem::copy_option::overwrite_if_exists );
  }

  path operator/( const path & prefix, const path & suffix )
  {
    return prefix.string() + "/" + suffix.string();
  }

  void create_directories( const path & pathn )
  {
    if ( not boost::filesystem::create_directories( pathn.string() ) ) {
      throw runtime_error( "could not create directory: " + pathn.string() );
    }
  }

  bool is_directory( const path & pathn )
  {
    return boost::filesystem::is_directory( pathn.string() );
  }
}
