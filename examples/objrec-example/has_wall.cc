/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main( int argc, char * argv[] ) {
  if ( argc < 2 ) {
    cout << "Usage: " << argv[0] << " <inp-objrec-file>" << endl;
    return EXIT_FAILURE;
  }

  ifstream infile( argv[1] );
  ofstream was_found( "was_found.out", ofstream::out );
  string line;
  bool found = false;
  while ( getline( infile, line ) && !found ) {
    string object = line.substr( 0, line.find( " (" ) );
    if ( object == "stone wall" ) {
      was_found << "1\n";
      found = true;
    }
  }

  if ( !found ) {
    was_found << "0\n";
  }

  infile.close();
  was_found.close();

  return EXIT_SUCCESS;
}

