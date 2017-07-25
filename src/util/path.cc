/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "path.hh"

using namespace std;

namespace roost {
  path::path( const std::string & pathn )
    : path_( pathn )
  {}

  path::path( const boost::filesystem::path & boost_path_ ) /* XXX */
    : path_( boost_path_ )
  {}

  path path::lexically_normal() const
  {
    return path_.lexically_normal();
  }

  const string & path::string() const
  {
    return path_.string();
  }

  const boost::filesystem::path & path::boost_path() const /* XXX */
  {
    return path_;
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
    return boost::filesystem::absolute( pathn.boost_path() );
  }

  void copy_file( const path & src, const path & dst )
  {
    return boost::filesystem::copy_file( src.boost_path(),
					 dst.boost_path(),
					 boost::filesystem::copy_option::overwrite_if_exists );
  }

  path operator/( const path & prefix, const path & suffix )
  {
    return prefix.boost_path() / suffix.boost_path();
  }
}
