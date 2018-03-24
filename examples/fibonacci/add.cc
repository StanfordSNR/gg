/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <libgen.h>

using namespace std;

int main( int argc, char * argv[] )
{
  if ( argc != 3 ) {
    cerr << "usage: add <A> <B>" << endl;
    return EXIT_FAILURE;
  }

  long long num1, num2;

  ifstream fin_a { argv[ 1 ] };
  ifstream fin_b { argv[ 2 ] };
  ofstream fout { "out" };

  fin_a >> num1;
  fin_b >> num2;
  fout << ( num1 + num2 ) << endl;

  return EXIT_SUCCESS;
}
