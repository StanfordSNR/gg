/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <stdexcept>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <libgen.h>
#include <deque>
#include <climits>
#include <vector>
#include <string>
#include <sstream>
#include <memory>

#include "path.hh"
#include "exception.hh"
#include "tokenize.hh"
#include "file_descriptor.hh"
#include "temp_file.hh"

using namespace std;

namespace roost {
  Directory::Directory( const string & path )
    : fd_( CheckSystemCall( "open directory (" + path + ")",
                            open( path.c_str(),
                            O_DIRECTORY | O_CLOEXEC ) ) )
  {}

  Directory::Directory( const Directory & parent, const string & path )
    : fd_( CheckSystemCall( "openat directory (" + path + ")",
                            openat( parent.num(), path.c_str(),
                            O_DIRECTORY | O_CLOEXEC ) ) )
  {}

  Directory::~Directory()
  {
    try {
      CheckSystemCall( "close", close( fd_ ) );
    }
    catch ( const exception & e ) {}
  }

  path::path()
    : path_()
  {}

  path::path( const char * pathn )
    : path::path( ::string( pathn ) )
  {}

  path::path( const std::string & pathn )
    : path_( pathn )
  {}

  path path::lexically_normal() const
  {
    vector<std::string> components_vector = path_components();
    vector<std::string> normalized;

    if ( components_vector.empty() ) {
      return std::string();
    }

    if ( components_vector.at( 0 ).empty() ) {
      normalized.push_back( "" );
    }

    for ( const std::string & component : components_vector ) {
      if ( component == ".." ) {
        if ( normalized.size() == 0 or normalized.back() == ".." ) {
          normalized.push_back( component );
        }
        else if ( !normalized.back().empty() ) {
          normalized.pop_back();
        }
      }
      else if ( component != "." and not component.empty() ) {
        normalized.push_back( component );
      }
    }

    ostringstream path_oss;
    for ( const std::string & component : normalized ) {
      path_oss << component << "/";
    }

    std::string p = path_oss.str();

    if ( p.length() > 1 and p.back() == '/' ) {
      p.erase( p.length() - 1 );
    }

    return p;
  }

  const string & path::string() const
  {
    return path_;
  }

  bool exists( const path & pathn )
  {
    return not access( pathn.string().c_str(), F_OK );
  }

  /* XXX need to be careful about race conditions if file size
     changes between when this is called, and later copy */

  /* maybe could have a thunk sanity check at the end, making sure
     all sizes match the objects in the gg directory? */

  size_t file_size( const path & pathn )
  {
    struct stat file_info;
    CheckSystemCall( "stat " + pathn.string(),
                     stat( pathn.string().c_str(), &file_info ) );
    return file_info.st_size;
  }

  path canonical( const path & pathn )
  {
    char canonical_file_name[ PATH_MAX ];

    if ( nullptr == realpath( pathn.string().c_str(), canonical_file_name ) ) {
      throw unix_error( "realpath", errno );
    }

    return string( canonical_file_name );
  }

  path dirname( const path & pathn )
  {
    char path_cstr[ PATH_MAX ];
    strcpy( path_cstr, pathn.string().c_str() );
    return ::dirname( path_cstr );
  }

  void copy_file( const path & src, const path & dst )
  {
    FileDescriptor src_file { CheckSystemCall( "open (" + src.string() + ")",
                              open( src.string().c_str(), O_RDONLY | O_CLOEXEC ) ) };
    struct stat src_info;
    CheckSystemCall( "fstat", fstat( src_file.fd_num(), &src_info ) );

    if ( not S_ISREG( src_info.st_mode ) ) {
      throw runtime_error( src.string() + " is not a regular file" );
    }

    FileDescriptor dst_file { CheckSystemCall( "open (" + dst.string() + ")",
                              open( dst.string().c_str(),
                                    O_WRONLY | O_CREAT | O_EXCL | O_CLOEXEC,
                              src_info.st_mode ) ) };

    dst_file.write( src_file.read_exactly( src_info.st_size ) );
  }

  void move_file( const path & src, const path & dst )
  {
    copy_then_rename( src, dst );
    remove( src );
  }

  void copy_to_fd( const path & src, FileDescriptor & dst_file )
  {
    FileDescriptor src_file { CheckSystemCall( "open (" + src.string() + ")",
                              open( src.string().c_str(), O_RDONLY | O_CLOEXEC ) ) };
    struct stat src_info;
    CheckSystemCall( "fstat", fstat( src_file.fd_num(), &src_info ) );

    if ( not S_ISREG( src_info.st_mode ) ) {
      throw runtime_error( src.string() + " is not a regular file" );
    }

    dst_file.write( src_file.read_exactly( src_info.st_size ) );
    CheckSystemCall( "fchmod", fchmod( dst_file.fd_num(), src_info.st_mode ) );
  }

  void copy_then_rename( const path & src, const path & dst )
  {
    TempFile tmp_file { dst.string() };
    copy_to_fd( src, tmp_file.fd() );
    rename( tmp_file.name(), dst.string() );
  }

  path operator/( const path & prefix, const path & suffix )
  {
    if ( ( not prefix.string().empty() and prefix.string().back() == '/' ) or
         ( not suffix.string().empty() and suffix.string().front() == '/') ) {
      return prefix.string() + suffix.string();
    }
    else {
      return prefix.string() + "/" + suffix.string();
    }
  }

  void create_directories_relative( const Directory & parent_directory,
                                    const vector<string>::const_iterator & begin,
                                    const vector<string>::const_iterator & end )
  {
    if ( begin == end ) {
      return;
    }

    /* empty path component -> skip */
    if ( begin->empty() ) {
      return create_directories_relative( parent_directory, begin + 1, end );
    }

    try {
      CheckSystemCall( "mkdirat (" + *begin + ")",
                       mkdirat( parent_directory.num(),
                       begin->c_str(),
                       S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH ) );
    }
    catch ( const unix_error & e ) {
      if ( e.saved_errno() == EEXIST ) {
        /* okay */
      }
      else {
        throw;
      }
    }

    create_directories_relative( Directory( parent_directory, *begin ), begin + 1, end );
  }

  void create_directories( const path & pathn )
  {
    vector<string> components = pathn.path_components();
    if ( components.empty() ) {
      return;
    }

    if ( components.front().empty() ) {
      components.front() = "/";
    }

    create_directories_relative( Directory( "." ), components.begin(), components.end() );
  }

  bool is_directory( const path & pathn )
  {
    struct stat file_info;
    CheckSystemCall( "stat " + pathn.string(),
                     stat( pathn.string().c_str(), &file_info ) );
    return S_ISDIR( file_info.st_mode );
  }

  bool is_directory_at( const Directory & parent_directory, const path & pathn )
  {
    struct stat file_info;
    CheckSystemCall( "fstatat " + pathn.string(),
                     fstatat( parent_directory.num(), pathn.string().c_str(),
                              &file_info, 0 ) );
    return S_ISDIR( file_info.st_mode );
  }

  vector<string> path::path_components() const
  {
    return split( path_, "/" );
  }

  bool remove( const path & pathn )
  {
    if ( not exists( pathn ) ) {
      return false;
    }

    CheckSystemCall( "remove " + pathn.string(),
                     ::remove( pathn.string().c_str() ) );

    return true;
  }

  bool remove_at( const Directory & parent_directory, const path & pathn,
                  const bool is_directory )
  {
    CheckSystemCall( "unlinkat " + pathn.string(),
                     unlinkat( parent_directory.num(),
                               pathn.string().c_str(),
                               is_directory ? AT_REMOVEDIR : 0 ) );

    return true;
  }

  void empty_directory_relative( const Directory & parent_directory,
                                 const string & pathn )
  {
    Directory directory( parent_directory, pathn );

    shared_ptr<DIR> dir { fdopendir( directory.num() ), closedir };

    if ( dir.get() == NULL ) {
      throw unix_error( "fdopendir" );
    }

    struct dirent * entry = NULL;

    while ( ( errno = 0, entry = readdir( dir.get() ) ) != NULL ) {
      string name { entry->d_name };

      cerr << pathn << " / " << name << endl;

      if ( name == "." or name  == ".." ) {
        continue;
      }

      bool is_dir = is_directory_at( directory, name );

      if ( is_dir ) {
        empty_directory_relative( directory, name );
      }

      remove_at( directory, name, is_dir );
    }

    if ( errno ) {
      throw unix_error( "readdir" );
    }
  }

  void remove_directory( const path & pathn )
  {
    Directory directory { pathn.string() };
    empty_directory_relative( directory, "." );
    remove( pathn );
  }

  void symlink( const path & old_name, const path & new_name )
  {
    CheckSystemCall( "symlink", ::symlink( old_name.string().c_str(),
                                           new_name.string().c_str() ) );
  }

  void rename( const path & oldpath, const path & newpath )
  {
    CheckSystemCall( "rename", ::rename( oldpath.string().c_str(),
                                         newpath.string().c_str() ) );
  }

  vector<string> get_directory_listing( const path & pathn )
  {
    Directory directory( pathn.string() );
    shared_ptr<DIR> dir { fdopendir( directory.num() ), closedir };

    if ( dir.get() == NULL ) {
      throw unix_error( "fdopendir" );
    }

    vector<string> result;

    struct dirent * entry = NULL;
    while ( ( errno = 0, entry = readdir( dir.get() ) ) != NULL ) {
      string name { entry->d_name };
      if ( name != ".." and name != "." ) {
        result.push_back( name );
      }
    }

    if ( errno ) {
      throw unix_error( "readdir" );
    }

    return result;
  }

  void chmod( const path & pathn, mode_t mode )
  {
    CheckSystemCall( "chmod", ::chmod( pathn.string().c_str(), mode ) );
  }
}
