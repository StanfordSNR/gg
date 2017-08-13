/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <unistd.h>
#include <cstring>
#include <iostream>

#include "exception.hh"
#include "path.cc"
#include "placeholder.hh"
#include "paths.hh"
#include "system_runner.hh"

using namespace std;

void usage( const char * argv0 )
{
  cerr << argv0 << "THUNK [execution args]" << endl;
}

int main( int argc, char * argv[] )
{
  try {
    if ( argc <= 0 ) {
      abort();
    }

    if ( argc < 2 ) {
      usage( argv[ 0 ] );
      return EXIT_FAILURE;
    }

    const roost::path gg_path = gg::paths::blobs();
    const string thunk_filename = argv[ 1 ];
    const roost::path thunk_path = roost::canonical( thunk_filename );

    Optional<ThunkPlaceholder> placeholder = ThunkPlaceholder::read( thunk_path.string() );

    if ( placeholder.initialized() ) {
      copy_then_rename( gg_path / placeholder->content_hash(), thunk_path );
    }

    vector<string> args = { "gg-reduce", thunk_filename };
    run( "gg-reduce", args, {}, true, true );
    CheckSystemCall( "execv", execv( thunk_path.string().c_str(), ++argv ) );
  }
  catch ( const exception &  e ) {
    print_exception( argv[ 0 ], e );
    return EXIT_FAILURE;
  }
}
