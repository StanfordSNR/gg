/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <iostream>
#include <vector>

#include "net/requests.hh"
#include "storage/backend.hh"
#include "thunk/ggutils.cc"

using namespace std;

void usage( char * argv0 )
{
  cerr << argv0 << " FILENAME..." << endl;
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

    vector<storage::PutRequest> put_requests;
    for ( int i = 1; i < argc; i++ ) {
      const string filename { argv[ i ] };
      string file_hash = gg::hash::file( filename );
      put_requests.emplace_back( filename, file_hash, gg::hash::to_hex( file_hash ) );
    }
    storage_backend->put( put_requests,
      []( const storage::PutRequest & request )
      {
        cerr << request.filename.string() << " -> "
             << request.object_key << endl;
      } );
  }
  catch ( const exception &  e ) {
    print_exception( argv[ 0 ], e );
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
