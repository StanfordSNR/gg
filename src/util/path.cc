/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <stdexcept>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <deque>

#include "path.hh"
#include "exception.hh"
#include "tokenize.hh"
#include "file_descriptor.hh"

#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem.hpp>
#undef BOOST_NO_CXX11_SCOPED_ENUMS

using namespace std;

namespace roost {
  class Directory
  {
  private:
    int fd_;

  public:
    Directory( const string & path )
      : fd_( CheckSystemCall( "open directory (" + path + ")",
			      open( path.c_str(), O_DIRECTORY | O_PATH | O_CLOEXEC ) ) )
    {}

    Directory( const Directory & parent, const string & path )
      : fd_( CheckSystemCall( "openat directory (" + path + ")",
			      openat( parent.num(), path.c_str(), O_DIRECTORY | O_PATH | O_CLOEXEC ) ) )
    {}

    ~Directory()
    {
      try {
	CheckSystemCall( "close", close( fd_ ) );
      } catch ( const exception & e ) {}
    }

    int num() const { return fd_; }
  };
  
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
    FileDescriptor src_file { CheckSystemCall( "open (" + src.string() + ")",
					       open( src.string().c_str(), O_RDONLY | O_CLOEXEC ) ) };
    struct stat src_info;
    CheckSystemCall( "fstat", fstat( src_file.fd_num(), &src_info ) );

    if ( not S_ISREG( src_info.st_mode ) ) {
      throw runtime_error( src.string() + " is not a regular file" );
    }

    FileDescriptor dst_file { CheckSystemCall( "open (" + dst.string() + ")",
					       open( dst.string().c_str(), O_WRONLY | O_CREAT | O_EXCL | O_CLOEXEC,
						     src_info.st_mode ) ) };

    dst_file.write( src_file.read_exactly( src_info.st_size ) );
  }

  path operator/( const path & prefix, const path & suffix )
  {
    return prefix.string() + "/" + suffix.string();
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
				S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) );
    } catch ( const unix_error & e ) {
      if ( e.saved_errno() == EEXIST ) {
	/* okay */
      } else {
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

  vector<string> path::path_components() const
  {
    return split( path_, "/" );
  }
}
