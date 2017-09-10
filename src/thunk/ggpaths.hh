/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef PATHS_HH
#define PATHS_HH

#include <string>
#include <stdexcept>
#include <vector>
#include <sys/types.h>

#include "path.hh"
#include "optional.hh"

namespace gg {
  namespace paths {
    roost::path blobs();
    roost::path reductions();
    roost::path remote_index();
    roost::path hash_cache();
    roost::path preprocessor_dependency_cache();

    roost::path blob_path( const std::string & hash );
    roost::path reduction_path( const std::string & hash );
    roost::path hash_cache_entry( const std::string & filename, const struct stat & stat_entry );
    roost::path preprocessor_dependency_cache_entry( const std::string & input_hash );

    void fix_path_envar();
  }

  namespace remote {
    bool is_available( const std::string & hash );
    void set_available( const std::string & hash );

    std::string s3_bucket();
    std::string s3_region();
  }

  namespace cache {
    struct ReductionResult
    {
      std::string hash;
      size_t order;
      /* XXX add this: off_t size; */
    };

    Optional<ReductionResult> check( const std::string & thunk_hash );
    void insert( const std::string & old_hash, const std::string & new_hash );
  }

  namespace hash {
    uint32_t extract_size( const std::string & hash );
  }

  namespace models {
    void init();
    static const std::string OPEN_TO_DETACH_PATH = "/__gg__detach_from_tracing__/";
    std::vector<std::string> args_to_vector( int argc, char ** argv );
  }
}

#endif /* PATHS_HH */
