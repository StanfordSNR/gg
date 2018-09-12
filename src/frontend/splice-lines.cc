/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <iostream>
#include <string>
#include <cctype>

using namespace std;

int main()
{
  string line;

  while ( getline( cin, line ) ) {
    if ( line == "#pragma once" ) {
      cout << "--GGKEEP--" << line << endl;
    }
    else if ( line.back() == '\\' ) {
      line.pop_back();
      cout << line;
    }
    else {
      cout << line << endl;
    }
  }

  return 0;
}
