/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef PATH_HH
#define PATH_HH

#include <string>
#include <boost/filesystem.hpp>

namespace roost {
  class Path
  {
  private:
    boost::filesystem::path path_;

  public:
    Path( const std::string & path );
    
    Path lexically_normal() const;
    const std::string & string() const;

    Path( const boost::filesystem::path & boost_path_ ); /* XXX */
    const boost::filesystem::path & boost_path() const; /* XXX */
  };

  bool exists( const Path & Path );
  size_t file_size( const Path & path );
  Path absolute( const Path & path );
  void copy_file( const Path & src, const Path & dest );
}

#endif /* PATH_HH */
