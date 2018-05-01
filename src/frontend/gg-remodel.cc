/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <iostream>
#include <stdexcept>
#include <map>
#include <tuple>
#include <sys/stat.h>

#include "protobufs/meta.pb.h"
#include "protobufs/util.hh"
#include "thunk/placeholder.hh"
#include "thunk/thunk.hh"
#include "thunk/ggutils.hh"
#include "thunk/metadata.hh"
#include "util/exception.hh"
#include "util/path.hh"
#include "util/system_runner.hh"

using namespace std;
using namespace gg;
using namespace gg::thunk;

roost::path base_path;

void usage( const char * argv0 )
{
  cerr << argv0 << " THUNK-PLACEHOLDER..." << endl;
}

string hash_file( const string & filename )
{
  typedef tuple<string, off_t, time_t, long, time_t, long> NodeInfo;
  static map<string, NodeInfo> hash_cache;

  /* do we have this hash in cache? */
  struct stat file_stat;
  CheckSystemCall( "stat", stat( filename.c_str(), &file_stat ) );

  if ( hash_cache.count( filename ) ) {
    NodeInfo & info = hash_cache[ filename ];

    if ( get<1>( info ) == file_stat.st_size and
         get<2>( info ) == file_stat.st_mtim.tv_sec and
         get<3>( info ) == file_stat.st_mtim.tv_nsec and
         get<4>( info ) == file_stat.st_ctim.tv_sec and
         get<5>( info ) == file_stat.st_ctim.tv_nsec ) {
      return get<0>( info );
    }
  }

  /* okay, let just cache it and save it */
  const string hash = gg::hash::file( filename );
  hash_cache[ filename ] = make_tuple( hash,
                                       file_stat.st_size,
                                       file_stat.st_mtim.tv_sec,
                                       file_stat.st_mtim.tv_nsec,
                                       file_stat.st_ctim.tv_sec,
                                       file_stat.st_ctim.tv_nsec );

  return hash;
}

bool matches_filesystem( const Thunk::DataItem & item )
{
  const string & hash = item.first;
  const string & filename = item.second;

  if ( filename.length() == 0 ) {
    return false;
  }

  if ( not roost::exists( filename ) ) {
    return false;
  }

  if ( gg::hash::size( hash ) != roost::file_size( filename ) ) {
    return false;
  }

  return hash == hash_file( filename );
}

set<string> remodeled_placeholders;

bool remodel( const string & path )
{
  const auto placeholder_canonical_path = roost::canonical( path ).string();

  if ( remodeled_placeholders.count( placeholder_canonical_path ) ) {
    return false;
  }

  auto placeholder = ThunkPlaceholder::read( path );

  if ( not placeholder.initialized() ) {
    throw runtime_error( "not a placeholder: " + path );
  }

  if ( placeholder->metadata().length() == 0 ) {
    throw runtime_error( "no metadata found: " + path );
  }

  // cerr << "Remodeling " << path << " (" << placeholder->content_hash() << ")"
  //     << endl;

  PlaceholderMetadata metadata { placeholder->metadata() };

  /* changing the working directory of the placeholder */
  auto target_dir = base_path / metadata.cwd();
  roost::chdir( target_dir );

  bool something_changed = false;

  for ( const ThunkFactory::Data & object : metadata.objects() ) {
    if ( object.type() == ObjectType::Thunk ) {
      something_changed |= remodel( object.filename() );

      /* let's go back to our directory */
      roost::chdir( target_dir );
    }
    else if ( not something_changed ) {
      if ( not matches_filesystem( { object.hash(), object.filename() } ) ) {
        /* it seems that something's changed, we need to re-run the model */
        something_changed = true;
      }
    }
  }

  if ( something_changed ) {
    vector<string> command { "gg-infer" };
    command.insert( command.end(), metadata.args().begin(), metadata.args().end() );
    run( command[ 0 ], command, {}, true, true );
    cerr << ">> remodeled: " << path << endl;
  }

  remodeled_placeholders.insert( placeholder_canonical_path );
  return something_changed;
}

int main( int argc, char ** argv )
{
  try {
    if ( argc <= 0 ) {
      abort();
    }

    if ( argc < 2 ) {
      usage( argv[ 0 ] );
      return EXIT_FAILURE;
    }

    setenv( "GG_REMODELING", "1", true );
    setenv( "GG_METAINFER", "1", true );
    setenv( "GG_BASEPATH", roost::current_working_directory().string().c_str(), true );

    base_path = roost::current_working_directory();

    vector<string> target_placeholders;

    for ( int i = 1; i < argc; i++ ) {
      target_placeholders.emplace_back( argv[ i ] );
    }

    for ( const auto & target_placeholder : target_placeholders ) {
      roost::chdir( base_path );
      remodel( target_placeholder );
    }

    return EXIT_SUCCESS;
  }
  catch ( const exception &  e ) {
    print_exception( argv[ 0 ], e );
    return EXIT_FAILURE;
  }
}
