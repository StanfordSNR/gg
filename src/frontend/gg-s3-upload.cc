/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <cstdlib>
#include <string>
#include <iostream>
#include <vector>

#include "net/s3.hh"

using namespace std;

int main()
{
  vector<storage::PutRequest> files;

  string filename;
  while ( cin >> filename ) {
    files.push_back( { filename, roost::rbasename( filename ).string(), "" } );
  }

  S3Client s3_client { {} };
  s3_client.upload_files( "ggfunbucket", files,
    [] ( const storage::PutRequest & request )
    {
      cout << "Upload done: " + request.filename.string() + "\n";
    }
  );

  return EXIT_SUCCESS;
}
