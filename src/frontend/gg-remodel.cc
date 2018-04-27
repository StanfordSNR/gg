/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <iostream>
#include <stdexcept>

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
  cerr << argv0 << " THUNK-PLACEHOLDER" << endl;
}

bool remodel( const string & path )
{
  auto placeholder = ThunkPlaceholder::read( path );
  if ( not placeholder.initialized() ) {
    throw runtime_error( "not a placeholder: " + path );
  }

  if ( placeholder->metadata().length() == 0 ) {
    throw runtime_error( "no metadata found: " + path );
  }

  cerr << "Remodeling " << path << " (" << placeholder->content_hash() << ")"
       << endl;

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
      if ( not Thunk::matches_filesystem( { object.hash(), object.filename() } ) ) {
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

  return something_changed;
}

int main( int argc, char ** argv )
{
  try {
    if ( argc <= 0 ) {
      abort();
    }

    if ( argc != 2 ) {
      usage( argv[ 0 ] );
      return EXIT_FAILURE;
    }

    base_path = roost::current_working_directory();
    const string target_placeholder = argv[ 1 ];

    remodel( target_placeholder );

    return EXIT_SUCCESS;
  }
  catch ( const exception &  e ) {
    print_exception( argv[ 0 ], e );
    return EXIT_FAILURE;
  }
}
