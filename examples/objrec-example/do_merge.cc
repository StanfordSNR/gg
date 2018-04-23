/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main( int argc, char * argv[] ) {
  if ( argc < 2 ) {
    cout << "Usage: " << argv[0] << " file1 file2 ..." << endl;
    return EXIT_FAILURE;
  }

  int num_found = 0;
  for ( uint8_t i = 1; i < argc; ++i ) {
    ifstream infile( argv[i] );
    int was_found;
    infile >> was_found;
    infile.close();
    num_found += was_found;
  }

  ofstream found_fd( "all_found.out" );
  found_fd << num_found << '\n';
  found_fd.close();

  return EXIT_SUCCESS;
}

