/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef PATH_HH
#define PATH_HH

#include <string>

#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem.hpp>
#undef BOOST_NO_CXX11_SCOPED_ENUMS

namespace roost {
  class path
  {
  private:
    std::string path_;

  public:
    path( const std::string & pathn );
    
    path lexically_normal() const;
    const std::string & string() const;

    path( const boost::filesystem::path & boost_path_ ); /* XXX */
    boost::filesystem::path boost_path() const; /* XXX */
  };

  bool exists( const path & pathn );
  size_t file_size( const path & pathn );
  path absolute( const path & pathn );
  void copy_file( const path & src, const path & dest );
  path operator/( const path & prefix, const path & suffix );
  path canonical( const path & pathn );
  void create_directories( const path & pathn );
  bool is_directory( const path & pathn );
}

#endif /* PATH_HH */
