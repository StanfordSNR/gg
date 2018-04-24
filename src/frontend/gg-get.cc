/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <iostream>
#include <vector>

#include "net/requests.hh"
#include "storage/backend.hh"
#include "thunk/ggutils.cc"

using namespace std;

void usage( char * argv0 )
{
  cerr << argv0 << " HASH..." << endl;
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

    unique_ptr<StorageBackend> storage_backend = StorageBackend::create_backend( gg::remote::storage_backend_uri() );

    vector<storage::GetRequest> get_requests;
    for ( int i = 1; i < argc; i++ ) {
      const string hash { argv[ i ] };
      get_requests.emplace_back( hash, gg::paths::blob( hash ) );
    }
    storage_backend->get( get_requests,
      []( const storage::GetRequest & request )
      {
        cerr << "GET " << request.object_key << " -> "
             << request.filename.string() << endl;
      } );
  }
  catch ( const exception &  e ) {
    print_exception( argv[ 0 ], e );
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
