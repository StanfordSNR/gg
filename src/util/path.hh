/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef PATH_HH
#define PATH_HH

#include <string>
#include <vector>
#include <sys/stat.h>

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
    path();
    path( const char * pathn );
    path( const std::string & pathn );

    path lexically_normal() const;
    const std::string & string() const;
    std::vector<std::string> path_components() const;
    bool empty() const { return ( path_.length() == 0 ); }

    bool operator==( const path & other ) const;
    bool operator!=( const path & other ) const { return not operator==( other ); }
  };

  bool exists( const path & pathn );
  off_t file_size( const path & pathn );
  void move_file( const path & src, const path & dest );
  path operator/( const path & prefix, const path & suffix );
  path canonical( const path & pathn );
  path dirname( const path & pathn );
  path rbasename( const path & pathn );
  void symlink( const path & target, const path & linkpath );
  path current_working_directory();
  void create_directories( const path & pathn );
  bool is_directory( const path & pathn );
  bool exists_and_is_directory( const path & pathn );
  bool remove( const path & pathn );
  bool remove_at( const Directory & directory, const path & pathn,
                  const bool is_directory = false );
  void remove_directory( const path & pathn );
  void empty_directory( const path & pathn );
  std::vector<std::string> list_directory( const path & pathn );
  void rename( const path & oldpath, const path & newpath );
  void chmod( const path & pathn, mode_t mode );
  void chdir( const path & pathn );
  std::string readlink( const path & pathn );
  std::vector<std::string> get_directory_listing( const path & pathn );
  bool is_absolute( const path & pathn );
  void make_executable( const path & pathn );
  bool is_executable( const path & pathn );
  std::string read_file( const path & pathn );
  void copy_then_rename( const path & src, const path & dest,
                         const bool set_mode = false, const mode_t target_mode = 0 );
  void atomic_create( const std::string & contents, const path & dst,
                      const bool set_mode = false, const mode_t target_mode = 0 );
}

#endif /* PATH_HH */
