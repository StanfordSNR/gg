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
    cout << "Usage: " << argv[0] << " <file-to-reduce> <out-file-name>" << endl;
    return EXIT_FAILURE;
  }

  ifstream input_file( argv[1] );
  ofstream out_file( argv[2] );

  string line;
  while ( getline( input_file, line ) ) {
    istringstream iss( line );
    vector<string> tokens{ istream_iterator<string>{iss}, istream_iterator<string>{} };

    auto tok_iter = tokens.begin();
    /* First is the key, rest are the values to reduce */
    string key_word = *tok_iter;
    tok_iter++;

    int counter = 0;
    while ( tok_iter != tokens.end() ) {
      counter += stoi( *tok_iter );
      tok_iter++;
    }

    /* Write reduced value */
    out_file << key_word << " " << counter << "\n";
  }
  input_file.close();
  out_file.close();

  return EXIT_SUCCESS;
}

