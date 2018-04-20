/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iterator>

using namespace std;

int main( int argc, char * argv[] ) {
  if ( argc < 3 ) {
    cout << "Usage: " << argv[0] << " <inp-file> <out-file-name>" << endl;
    return EXIT_FAILURE;
  }

  ifstream inp_file( argv[1] );
  ofstream out_file( argv[2] );

  string line;
  while ( getline( inp_file, line ) ) {
    istringstream iss( line );
    vector<string> tokens{ istream_iterator<string>{iss}, istream_iterator<string>{} };
    for ( string s : tokens ) {
      out_file << s << " " << 1 << '\n';
    }
  }
  inp_file.close();
  out_file.close();

  return EXIT_SUCCESS;
}

    
