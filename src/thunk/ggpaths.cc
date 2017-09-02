/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "ggpaths.hh"

#include <fstream>

#include "util.hh"

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

    roost::path get_blobs_path()
    {
      return get_gg_dir();
    }

    roost::path get_reductions_path()
    {
      roost::path reductions_dir = get_gg_dir() / "reductions";

      if ( roost::exists( reductions_dir ) ) {
        if ( not roost::is_directory( reductions_dir ) ) {
          throw runtime_error( reductions_dir.string() + " is not a directory" );
        }
      } else {
        roost::create_directories( reductions_dir );
      }

      return reductions_dir;
    }

    roost::path get_remote_index()
    {
      roost::path index_dir = get_gg_dir() / "remote";

      if ( roost::exists( index_dir ) ) {
        if ( not roost::is_directory( index_dir ) ) {
          throw runtime_error( index_dir.string() + " is not a directory" );
        }
      } else {
        roost::create_directories( index_dir );
      }

      return index_dir;
    }

    roost::path blobs()
    {
      const static roost::path blobs_path = get_blobs_path();
      return blobs_path;
    }

    roost::path reductions()
    {
      const static roost::path reductions_path = get_reductions_path();
      return reductions_path;
    }

    roost::path remote_index()
    {
      const static roost::path index_path = get_remote_index();
      return index_path;
    }

    roost::path blob_path( const string & hash )
    {
      return blobs() / hash;
    }

    roost::path reduction_path( const string & hash )
    {
      return reductions() / hash;
    }
  }

  namespace remote {
    bool is_available( const std::string & hash )
    {
      return roost::exists( gg::paths::remote_index() / hash );
    }

    void set_available( const std::string & hash )
    {
      ofstream fout { ( gg::paths::remote_index() / hash ).string() };
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

      ifstream fin { reduction.string() };

      string output_hash;
      fin >> output_hash;

      return ReductionResult { output_hash, 0 };
    }

    void insert( const string & old_hash, const string & new_hash )
    {
      ofstream fout { gg::paths::reduction_path( old_hash ).string(),
                      ios::out | ios::trunc };

      fout << new_hash;
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
