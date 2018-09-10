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

    bool read_names_from_stdin = false;
    constexpr size_t BATCH_SIZE = 64 * 32;

    if ( argc == 1 ) {
      read_names_from_stdin = true;
    }

    unique_ptr<StorageBackend> storage_backend = StorageBackend::create_backend( gg::remote::storage_backend_uri() );

    vector<storage::PutRequest> put_requests;
    int arg_index = 1;
    bool last_batch = false;
    string line;

    while ( not last_batch ) {
      string file_name;
      string file_hash;

      if ( read_names_from_stdin ) {
        if ( getline( cin, line ) ) {
          file_name = line;
        }
        else {
          last_batch = true;
        }
      }
      else {
        if ( arg_index < argc ) {
          file_name = argv[ arg_index++ ];
        }
        else {
          last_batch = true;
        }
      }

      if ( not last_batch ) {
        file_hash = gg::hash::file_force( file_name );
        if ( not storage_backend->is_available( file_hash ) ) {
          put_requests.emplace_back( file_name, file_hash, gg::hash::to_hex( file_hash ) );
        }
      }

      if ( put_requests.size() >= BATCH_SIZE or
           ( put_requests.size() > 0 and last_batch ) ) {
        storage_backend->put( put_requests,
          [&storage_backend]( const storage::PutRequest & request ) {
            storage_backend->set_available( request.object_key );
            cerr << "PUT " << request.filename.string() << " -> " << request.object_key << endl;
          } );

        put_requests.clear();
      }
    }
  }
  catch ( const exception &  e ) {
    print_exception( argv[ 0 ], e );
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
