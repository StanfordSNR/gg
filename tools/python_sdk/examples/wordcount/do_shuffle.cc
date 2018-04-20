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
  if ( argc < 2 ) {
    cout << "Usage: " << argv[0] << " <file1> <file2> ... <num-reducers>" << endl;
    return EXIT_FAILURE;
  }

  int num_reducers = atoi( argv[argc - 1] );

  map<string, vector<string>> red_map;

  /* Create map for reduce jobs */
  for ( int i = 1; i < argc - 1; ++i ) {
    ifstream infile( argv[i] );
    string next_word, next_count;
    while ( infile >> next_word >> next_count ) {
      if ( red_map.find( next_word ) == red_map.end() ) {
        vector<string> next_vec;
        next_vec.push_back( next_count );
        red_map.insert( make_pair( next_word, next_vec ) );
      }
      else {
        red_map[next_word].push_back( next_count );
      }
    }
    infile.close();
  }

  int words_per_output = ceil( static_cast<double> ( red_map.size() ) / 
                               static_cast<double> ( num_reducers ) );

  auto map_iter = red_map.begin();

  for ( int i = 0; i < num_reducers; ++i ) {
    ofstream out_file( "shuffle_out" + to_string( i ) + ".out" );
    for ( int j = 0; j < words_per_output; ++j ) {
      out_file << map_iter->first << " ";
      ostringstream oss;
      ostream_iterator<string> output_iterator( oss, " " );
      copy( map_iter->second.begin(), 
            map_iter->second.end() - 1, 
            ostream_iterator<string>( oss, " " ) );
      oss << map_iter->second.back();
      out_file << oss.str() << "\n";
      map_iter++;
      if ( map_iter == red_map.end() ) {
        break;
      }
    }
    out_file.close();
  }

  return EXIT_SUCCESS;
}

    
