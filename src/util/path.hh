/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef PATH_HH
#define PATH_HH

#include <string>
#include <vector>

namespace roost {
  class Directory
  {
  private:
    int fd_;

  public:
    Directory( const std::string & path );
    Directory( const Directory & parent, const std::string & path );

    ~Directory();

    int num() const { return fd_; }
  };

  class path
  {
  private:
    std::string path_;

  public:
    path( const char * pathn );
    path( const std::string & pathn );

    path lexically_normal() const;
    const std::string & string() const;
    std::vector<std::string> path_components() const;
  };

  bool exists( const path & pathn );
  size_t file_size( const path & pathn );
  void copy_file( const path & src, const path & dest );
  void move_file( const path & src, const path & dest );
  path operator/( const path & prefix, const path & suffix );
  path canonical( const path & pathn );
  path dirname( const path & pathn );
  void create_directories( const path & pathn );
  bool is_directory( const path & pathn );
  bool remove( const path & pathn );
  bool remove_at( const Directory & directory, const path & pathn,
                  const bool is_directory = false );
  void remove_directory( const path & pathn );
  void symlink( const path & old_name, const path & new_name );
  std::vector<std::string> get_directory_listing( const path & pathn );
}

#endif /* PATH_HH */
