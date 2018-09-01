/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <iostream>
#include <string>
#include <cctype>

using namespace std;

int main()
{
  string line;
  bool keep_next_line = false;

  while ( getline( cin, line ) ) {
    if ( keep_next_line ) {
      cout << line << endl;
      keep_next_line = ( line.back() == '\\' );
      continue;
    }

    for ( size_t i = 0; i < line.length(); i++ ) {
      char c = line [ i ];

      if ( c == '#' ) {
        cout << line << endl;
        keep_next_line = ( line.back() == '\\' );
      }
      else if ( isspace( c ) ) {
        continue;
      }

      break;
    }
  }

  return 0;
}
