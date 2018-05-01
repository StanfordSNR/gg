/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <iostream>
#include <stdexcept>
#include <map>
#include <tuple>

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

set<string> files;

void usage( const char * argv0 )
{
  cerr << argv0 << " THUNK-PLACEHOLDER..." << endl;
}

void add_children( const string & path )
{
  auto placeholder = ThunkPlaceholder::read( path );

  if ( not placeholder.initialized() ) {
    throw runtime_error( "not a placeholder: " + path );
  }

  if ( placeholder->metadata().length() == 0 ) {
    throw runtime_error( "no metadata found: " + path );
  }

  files.insert( roost::canonical( path ).string() );

  PlaceholderMetadata metadata { placeholder->metadata() };

  /* changing the working directory of the placeholder */
  auto target_dir = base_path / metadata.cwd();
  roost::chdir( target_dir );

  for ( const ThunkFactory::Data & object : metadata.objects() ) {
    if ( object.type() == ObjectType::Thunk ) {
      add_children( object.filename() );

      /* let's go back to our directory */
      roost::chdir( target_dir );
    }
    else {
      if ( object.filename().at( 0 ) != '/' ) {
        files.insert( roost::canonical( object.filename() ).string() );
      }
    }
  }
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

    base_path = roost::current_working_directory();
    const size_t base_path_length = base_path.string().length() + 1;

    vector<string> target_placeholders;

    for ( int i = 1; i < argc; i++ ) {
      target_placeholders.emplace_back( argv[ i ] );
    }

    for ( const auto & target_placeholder : target_placeholders ) {
      roost::chdir( base_path );
      add_children( target_placeholder );
    }

    for ( const auto & file : files ) {
      cout << file.substr( base_path_length ) << endl;
    }

    for ( const auto & blob : list_directory( gg::paths::blobs() ) ) {
      if ( blob.length() == 0 or blob == "." or blob == ".." ) {
        continue;
      }

      if ( blob[ 0 ] == 'T' ) {
        cout << roost::canonical( gg::paths::blob( blob ) ).string().substr( base_path_length ) << endl;
      }
    }

    for ( const auto & remote : list_directory( gg::paths::remote_index() ) ) {
      if ( remote.length() == 0 or remote == "." or remote == ".." ) {
        continue;
      }

      cout << roost::canonical( gg::paths::remote_index() / remote ).string().substr( base_path_length ) << endl;
    }

    for ( const auto & reduction : list_directory( gg::paths::reductions() ) ) {
      if ( reduction.length() == 0 or reduction == "." or reduction == ".." ) {
        continue;
      }

      cout << roost::canonical( gg::paths::reductions() / reduction ).string().substr( base_path_length ) << endl;
    }

    return EXIT_SUCCESS;
  }
  catch ( const exception &  e ) {
    print_exception( argv[ 0 ], e );
    return EXIT_FAILURE;
  }
}
