/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <cstdlib>
#include <string>
#include <iostream>
#include <vector>

#include "s3.hh"
#include "path.hh"
#include "ggpaths.hh"

using namespace std;

void usage( const char * argv0 )
{
  cerr << argv0 << " BLOB-HASH [output-file]" << endl;
}

int main( int argc, char * argv[] )
{
  if ( argc <= 0 ) {
    abort();
  }

  if ( argc < 2 ) {
    usage( argv[ 0 ] );
    return EXIT_FAILURE;
  }

  string blob_hash { argv[ 1 ] };
  roost::path output_path = gg::paths::blob_path( blob_hash );

  if ( argc == 3 ) {
    output_path = argv[ 2 ];
  }

  S3ClientConfig client_config;
  client_config.region = gg::remote::s3_region();

  S3Client s3_client;
  s3_client.download_file( gg::remote::s3_bucket(), blob_hash, output_path );

  return EXIT_SUCCESS;
}
