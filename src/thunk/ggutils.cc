/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "ggutils.hh"

#include <sstream>
#include <iomanip>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <fcntl.h>
#include <unistd.h>
#include <crypto++/sha.h>
#include <crypto++/hex.h>
#include <crypto++/base64.h>

#include "util/digest.hh"
#include "util/exception.hh"
#include "util/file_descriptor.hh"
#include "util/tokenize.hh"
#include "util/util.hh"

using namespace std;
using namespace CryptoPP;

namespace gg {
  namespace paths {
    roost::path get_gg_dir()
    {
      const char * envar = getenv( "GG_DIR" );

      if ( envar == nullptr ) {
        /* walk up the directory tree and look for .gg folder */
        roost::path cwd;
        roost::path new_cwd = roost::current_working_directory();

        do {
          cwd = new_cwd;
          roost::path path_candidate = cwd / ".gg";

          if ( roost::exists( path_candidate ) and roost::is_directory( path_candidate ) ) {
            return path_candidate;
          }
          else {
            new_cwd = roost::dirname( cwd );
          }
        } while( cwd != new_cwd );

        throw runtime_error( "GG_DIR not set, and could not find .gg in any of parent dirs" );
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

    roost::path get_inner_directory( const string & name )
    {
      const static roost::path gg_dir = get_gg_dir();
      roost::path inner_dir = gg_dir / name;

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
    bool is_available( const string & hash )
    {
      return roost::exists( gg::paths::remote_index() / hash );
    }

    void set_available( const string & hash )
    {
      atomic_create( "", gg::paths::remote_index() / hash );
    }

    string s3_bucket()
    {
      const static string bucket = safe_getenv( "GG_S3_BUCKET" );
      if ( bucket.length() == 0 ) {
        throw runtime_error( "GG_S3_BUCKET environment variable not set" );
      }
      return bucket;
    }

    string s3_region()
    {
      const static string bucket = safe_getenv( "GG_S3_REGION" );
      if ( bucket.length() == 0 ) {
        throw runtime_error( "GG_S3_REGION environment variable not set" );
      }
      return bucket;
    }

    std::string storage_backend_uri()
    {
      const static string uri = safe_getenv( "GG_STORAGE_URI" );
      if ( uri.length() == 0 ) {
        throw runtime_error( "GG_STORAGE_URI environment variable not set" );
      }
      return uri;
    }

    pair<string, uint16_t> runner_server()
    {
      const static string address = safe_getenv( "GG_RUNNER_SERVER" );
      if ( address.length() == 0 ) {
        throw runtime_error( "GG_RUNNER_SERVER environment variable not set" );
      }
      vector<string> data = split( address, ":" );

      return { data.at( 0 ), stoul( data.at( 1 ) ) };
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
      const string output_hash = cache_entry.read_exactly( gg::hash::length );

      return ReductionResult { output_hash };
    }

    void insert( const string & old_hash, const string & new_hash,
                 const string & tag )
    {
      if ( tag.size() == 0 ) {
        atomic_create( new_hash, gg::paths::reduction_path( old_hash ) );
      }
      else {
        string key = old_hash + "#" + tag;
        atomic_create( new_hash, gg::paths::reduction_path( key ) );
      }
    }
  }

  namespace hash {
    string for_output( const string & thunk_hash, const string & output_tag )
    {
      return thunk_hash + "#" + output_tag;
    }

    string compute( const string & input, const ObjectType type )
    {
      string ret { move( digest::sha256( input ) ) };
      ostringstream output_sstr;

      replace( ret.begin(), ret.end(), '-', '.' );
      output_sstr << to_underlying( type ) << ret << setfill( '0' )
                  << setw( 8 ) << hex << input.length();
      return output_sstr.str();
    }

    string to_hex( const string & gghash )
    {
      string output;

      string hash = gghash.substr( 1, gghash.length() - 9 );
      replace( hash.begin(), hash.end(), '.', '-' );
      hash += '=';

      StringSource s( hash, true,
                      new Base64URLDecoder(
                      new HexEncoder(
                      new StringSink( output ), false ) ) );

      if ( output.length() == 64 ) {
        return output;
      }
      else {
        throw runtime_error( "invalid gghash: " + gghash );
      }
    }

    uint32_t size( const string & hash )
    {
      assert( hash.length() >= 8 );
      return stoul( hash.substr( hash.length() - 8 ), nullptr, 16 );
    }

    ObjectType type( const string & hash )
    {
      if ( hash.length() == 0 ) {
        throw runtime_error( "invalid hash" );
      }

      switch ( hash[ 0 ] ) {
      case 'T': return ObjectType::Thunk;
      case 'V': return ObjectType::Value;
      default: throw runtime_error( "invalid hash type" );
      }
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

    vector<string> args_to_vector( int argc, char ** argv, const string & argv0 )
    {
      if ( argc < 1 ) {
        throw runtime_error( "no args provided" );
      }

      vector<string> result;
      ( argv0.length() > 0 ) ? result.push_back( argv0 )
                             : result.push_back( argv[ 0 ] );

      /* ignores argv[ 0 ] */
      for ( int i = 1; i < argc; i++ ) {
        result.push_back( argv[ i ] );
      }

      return result;
    }
  }
}
