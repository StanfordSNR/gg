/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef PATHS_HH
#define PATHS_HH

#include <string>
#include <stdexcept>
#include <vector>
#include <sys/types.h>

#include "manifest.hh"
#include "thunk.hh"
#include "util/path.hh"
#include "util/optional.hh"

namespace gg {
  namespace paths {
    roost::path root();
    roost::path blobs();
    roost::path reductions();
    roost::path metadata();
    roost::path remotes();
    roost::path hash_cache();
    roost::path dependency_cache();
    roost::path inclue_cache();
    roost::path blueprints();

    roost::path blob( const std::string & hash );
    roost::path reduction( const std::string & hash );
    roost::path metadata( const std::string & hash );
    roost::path remote( const std::string & hash );
    roost::path hash_cache_entry( const std::string & filename, const struct stat & stat_entry );
    roost::path dependency_cache_entry( const std::string & cache_key );
    roost::path include_cache_entry( const std::string & hash );
    roost::path blueprint( const std::string & hash );

    void fix_path_envar();
  }

  namespace remote {
    std::string storage_backend_uri();
  }

  namespace cache {
    struct ReductionResult
    {
      std::string hash;
    };

    Optional<ReductionResult> check( const std::string & thunk_hash );
    void insert( const std::string & old_hash, const std::string & new_hash );
  }

  namespace hash {
    constexpr size_t length = 1 /* type */
                            + 256 / 6 /* base64(sha256) */
                            + 1 /* round up */
                            + 8 /* length */;

    std::string base( const std::string & hash );
    std::string for_output( const std::string & thunk_hash, const std::string & output_tag );

    std::string compute( const std::string & input, const ObjectType type );
    std::string file( const roost::path & path, Optional<ObjectType> type = {} );
    std::string file_force( const roost::path & path, Optional<ObjectType> type = {} );
    std::string to_hex( const std::string & gghash );

    uint32_t size( const std::string & gghash );
    ObjectType type( const std::string & gghash );
  }

  namespace thunk {
    void store( const Thunk & thunk, const FileManifest & manifest );
  }

  namespace models {
    void init();
    static const std::string OPEN_TO_DETACH_PATH = "/__gg__detach_from_tracing__/";
    std::vector<std::string> args_to_vector( int argc, char ** argv, const std::string & argv0 = {} );
  }
}

#endif /* PATHS_HH */
