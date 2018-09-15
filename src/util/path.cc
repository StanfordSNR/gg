/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <stdexcept>
#include <string.h>
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
#include <errno.h>

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

  bool path::operator==( const path & other ) const
  {
    return other.path_ == path_;
  }

  bool exists( const path & pathn )
  {
    return not access( pathn.string().c_str(), F_OK );
  }

  /* XXX need to be careful about race conditions if file size
     changes between when this is called, and later copy */

  /* maybe could have a thunk sanity check at the end, making sure
     all sizes match the objects in the gg directory? */

  off_t file_size( const path & pathn )
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

  path rbasename( const path & pathn )
  {
    char path_cstr[ PATH_MAX ];
    strcpy( path_cstr, pathn.string().c_str() );
    return ::basename( path_cstr );
  }

  void symlink( const path & target, const path & linkpath )
  {
    CheckSystemCall( "symlink", ::symlink( target.string().c_str(),
                                           linkpath.string().c_str() ) );
  }

  path current_working_directory()
  {
    char path_cstr[ PATH_MAX ];
    if ( getcwd( path_cstr, sizeof( path_cstr ) ) == nullptr ) {
      throw unix_error( "cannot get current working directory" );
    }
    return { path_cstr };
  }

  void move_file( const path & src, const path & dst )
  {
    /* attempt simple rename (will work if on same filesystem) */
    const int rename_result = ::rename( src.string().c_str(),
                                        dst.string().c_str() );

    if ( rename_result == 0 ) {
      return;
    }
    else if ( errno != EXDEV ) {
      throw unix_error( "rename()" );
    }

    /* failed, so make copy onto target filesystem first */
    copy_then_rename( src, dst );
    remove( src );
  }

  void atomic_create( const string & contents, const path & dst,
                      const bool set_mode, const mode_t target_mode )
  {
    string tmp_file_name;
    {
      UniqueFile tmp_file { dst.string() };
      tmp_file_name = tmp_file.name();

      if ( contents.size() > 0 ) {
        tmp_file.fd().write( contents );
      }

      if ( set_mode ) {
        CheckSystemCall( "fchmod", fchmod( tmp_file.fd().fd_num(), target_mode ) );
      }

      /* allow block to end so the UniqueFile gets closed() before rename. */
      /* not 100% sure readers will see fully-written file appear atomically otherwise */
    }

    rename( tmp_file_name, dst.string() );
  }

  void copy_then_rename( const path & src, const path & dst,
                         const bool set_mode, const mode_t target_mode )
  {
    /* read input file into memory */
    FileDescriptor src_file { CheckSystemCall( "open (" + src.string() + ")",
                              open( src.string().c_str(), O_RDONLY ) ) };
    struct stat src_info;
    CheckSystemCall( "fstat", fstat( src_file.fd_num(), &src_info ) );

    if ( not S_ISREG( src_info.st_mode ) ) {
      throw runtime_error( src.string() + " is not a regular file" );
    }

    const string contents = src_file.read_exactly( src_info.st_size );

    /* write out to new file */
    atomic_create( contents, dst, true, set_mode ? target_mode : src_info.st_mode );
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
      if ( e.error_code() == EEXIST ) {
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
                              &file_info, AT_SYMLINK_NOFOLLOW ) );
    return S_ISDIR( file_info.st_mode );
  }

  bool exists_and_is_directory( const path & pathn )
  {
    struct stat file_info;
    if ( stat( pathn.string().c_str(), &file_info ) ) {
      return false;
    }
    return S_ISDIR( file_info.st_mode );
  }

  vector<string> path::path_components() const
  {
    return split( path_, "/" );
  }

  bool remove( const path & pathn )
  {
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

  void empty_directory_recursive( const Directory & parent_directory,
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

      if ( name == "." or name  == ".." ) {
        continue;
      }

      bool is_dir = is_directory_at( directory, name );

      if ( is_dir ) {
        empty_directory_recursive( directory, name );
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
    empty_directory_recursive( directory, "." );
    remove( pathn );
  }

  void empty_directory( const path & pathn )
  {
    Directory directory { pathn.string() };
    shared_ptr<DIR> dir { fdopendir( directory.num() ), closedir };

    if ( dir.get() == nullptr ) {
      throw unix_error( "fdopendir" );
    }

    struct dirent * entry = NULL;

    while ( ( errno = 0, entry = readdir( dir.get() ) ) != NULL ) {
      string name { entry->d_name };

      if ( name == "." or name  == ".." ) {
        continue;
      }

      bool is_dir = is_directory_at( directory, name );

      if ( not is_dir ) {
        remove_at( directory, name, is_dir );
      }
    }

    if ( errno ) {
      throw unix_error( "readdir" );
    }
  }

  vector<string> list_directory( const path & pathn )
  {
    shared_ptr<DIR> dir { opendir( pathn.string().c_str() ), closedir };
    struct dirent * entry = NULL;

    if ( not dir ) {
      throw runtime_error( "cannot open directory: " + pathn.string() );
    }

    vector<string> output;

    while ( ( entry = readdir( dir.get() ) ) != NULL ) {
      output.emplace_back( entry->d_name );
    }

    return output;
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

  void chdir( const path & pathn )
  {
    CheckSystemCall( "chdir", ::chdir( pathn.string().c_str() ) );
  }

  string read_file( const path & pathn )
  {
    /* read input file into memory */
    FileDescriptor in_file { CheckSystemCall( "open (" + pathn.string() + ")",
                              open( pathn.string().c_str(), O_RDONLY ) ) };
    struct stat pathn_info;
    CheckSystemCall( "fstat", fstat( in_file.fd_num(), &pathn_info ) );

    if ( not S_ISREG( pathn_info.st_mode ) ) {
      throw runtime_error( pathn.string() + " is not a regular file" );
    }

    return in_file.read_exactly( pathn_info.st_size );
  }

  void make_executable( const path & pathn )
  {
    struct stat file_info;
    CheckSystemCall( "stat " + pathn.string(),
                     stat( pathn.string().c_str(), &file_info ) );
    chmod( pathn, file_info.st_mode | S_IXUSR );
  }

  bool is_executable( const path & pathn )
  {
    struct stat file_info;
    CheckSystemCall( "stat", stat( pathn.string().c_str(), &file_info ) );
    return file_info.st_mode & S_IXUSR;
  }

  string readlink( const path & pathn )
  {
    char result[ PATH_MAX ];

    ssize_t len = CheckSystemCall( "readlink", ::readlink( pathn.string().c_str(), result, sizeof( result ) ) );
    result[ len ] = '\0';

    return { result };
  }

  bool is_absolute( const path & pathn )
  {
    if ( pathn.string().length() == 0 ) {
      return false;
    }

    return ( pathn.string().at( 0 ) == '/' );
  }
}
