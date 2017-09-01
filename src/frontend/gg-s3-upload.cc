/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <cstdlib>
#include <string>
#include <iostream>
#include <vector>

#include "s3.hh"

using namespace std;

int main()
{
  vector<pair<roost::path, string>> files;

  string filename;
  while ( cin >> filename ) {
    files.push_back( { filename, roost::rbasename( filename ).string() } );
  }

  S3Client s3_client;
  s3_client.upload_files( "ggfunbucket", files );

  return EXIT_SUCCESS;
}
