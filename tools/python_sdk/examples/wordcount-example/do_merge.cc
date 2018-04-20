/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <iterator>
#include <math.h>

using namespace std;

int main( int argc, char * argv[] ) {
  if ( argc < 3 ) {
    cout << "Usage: " << argv[0] << " <file1> <file2> ... <out-name>" << endl;
    return EXIT_FAILURE;
  }

  ofstream out_file( argv[argc-1] );

  /* Create map for reduce jobs */
  for ( int i = 1; i < argc - 1; ++i ) {
    ifstream infile( argv[i] );
    string next_word, next_count;
    while ( infile >> next_word >> next_count ) {
      out_file << next_word << " " << next_count << "\n";
    }
    infile.close();
  }
  out_file.close();

  return EXIT_SUCCESS;
}

    
