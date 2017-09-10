/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "ggpaths.hh"

#include <sys/types.h>
#include <sys/fcntl.h>
#include <fcntl.h>
#include <unistd.h>

#include "util.hh"
#include "file_descriptor.hh"
#include "exception.hh"
#include "digest.hh"

using namespace std;

namespace gg {
  namespace paths {
    roost::path get_gg_dir()
    {
      const char * envar = getenv( "GG_DIR" );

      if ( envar == NULL ) {
        throw runtime_error( "GG_DIR environment variable not set" );
      }

      roost::path gg_path { string( envar ) };

      if ( not roost::is_absolute( gg_path ) ) {
        throw runtime_error( "GG_DIR must be an absolute path" );
      }

      if ( roost::exists( gg_path ) ) {
        if ( not roost::is_directory( gg_path ) ) {
          throw runtime_error( gg_path.string() + " is not a directory" );
        }
      } else {
        roost::create_directories( gg_path );
      }

      return roost::canonical( gg_path );
    }

    roost::path get_inner_directory( const std::string & name )
    {
      roost::path inner_dir = get_gg_dir() / name;

      if ( roost::exists( inner_dir ) ) {
        if ( not roost::is_directory( inner_dir ) ) {
          throw runtime_error( inner_dir.string() + " is not a directory" );
        }
      } else {
        roost::create_directories( inner_dir );
      }

      return inner_dir;
    }

    roost::path blobs()
    {
      const static roost::path blobs_path = get_inner_directory( "blobs" );
      return blobs_path;
    }

    roost::path reductions()
    {
      const static roost::path reductions_path = get_inner_directory( "reductions" );
      return reductions_path;
    }

    roost::path remote_index()
    {
      const static roost::path index_path = get_inner_directory( "remote" );
      return index_path;
    }

    roost::path hash_cache()
    {
      const static roost::path hash_cache_path = get_inner_directory( "hash_cache" );
      return hash_cache_path;
    }

    roost::path dependency_cache()
    {
      const static roost::path cache_path = get_inner_directory( "depcache" );
      return cache_path;
    }

    roost::path blob_path( const string & hash )
    {
      return blobs() / hash;
    }

    roost::path reduction_path( const string & hash )
    {
      return reductions() / hash;
    }

    roost::path hash_cache_entry( const string & filename, const struct stat & stat_entry )
    {
      const string cache_key = to_string( stat_entry.st_dev ) + "-"
        + to_string( stat_entry.st_ino ) + "-" + roost::rbasename( filename ).string();
      return hash_cache() / cache_key;
    }

    roost::path dependency_cache_entry( const string & cache_key )
    {
      return dependency_cache() / cache_key;
    }

    void fix_path_envar()
    {
      if ( getenv( "GG_REALPATH" ) != nullptr ) {
        setenv( "PATH", getenv( "GG_REALPATH" ), true );
      }
    }
  }

  namespace remote {
    bool is_available( const std::string & hash )
    {
      return roost::exists( gg::paths::remote_index() / hash );
    }

    void set_available( const std::string & hash )
    {
      atomic_create( "", gg::paths::remote_index() / hash );
    }

    std::string s3_bucket()
    {
      const static string bucket = safe_getenv( "GG_S3_BUCKET" );
      if ( bucket.length() == 0 ) {
        throw runtime_error( "GG_S3_BUCKET environment variable not set" );
      }
      return bucket;
    }

    std::string s3_region()
    {
      const static string bucket = safe_getenv( "GG_S3_REGION" );
      if ( bucket.length() == 0 ) {
        throw runtime_error( "GG_S3_REGION environment variable not set" );
      }
      return bucket;
    }
  }

  namespace cache {
    Optional<ReductionResult> check( const string & thunk_hash )
    {
      roost::path reduction { gg::paths::reduction_path( thunk_hash ) };

      if ( not roost::exists( reduction ) ) {
        return {}; // no reductions are available
      }

      FileDescriptor cache_entry { CheckSystemCall( "open( " + reduction.string() + " )",
                                                    open( reduction.string().c_str(), O_RDONLY ) ) };
      const string output_hash = cache_entry.read_exactly( digest::length );

      return ReductionResult { output_hash, 0 };
    }

    void insert( const string & old_hash, const string & new_hash )
    {
      atomic_create( new_hash, gg::paths::reduction_path( old_hash ) );
    }
  }

  namespace hash {
    uint32_t extract_size( const string & hash )
    {
      assert( hash.length() >= 8 );
      return stoul( hash.substr( hash.length() - 8 ), nullptr, 16 );
    }
  }

  namespace models {
    void init()
    {
      int fd = open( gg::models::OPEN_TO_DETACH_PATH.c_str(), O_RDONLY );

      if ( fd >= 0 ) {
        close( fd );
      }

      gg::paths::fix_path_envar();
    }

    vector<string> args_to_vector( int argc, char ** argv )
    {
      vector<string> result;

      /* ignores argv[ 0 ] */
      for ( int i = 1; i < argc; i++ ) {
        result.push_back( argv[ i ] );
      }

      return result;
    }
  }
}
