/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>

using namespace std;

int main( int argc, char * argv[] )
{
  if ( argc != 3 ) {
    return EXIT_FAILURE;
  }

  const long long N = stoll( argv[ 1 ] );

  if ( N < 0 ) {
    cerr << argv[ 0 ] << " doesn't accept negative inputs" << endl;
    return EXIT_FAILURE;
  }

  ofstream fout { argv[ 2 ] };

  if ( N < 2 ) {
    /* in this case, we just return the value and our job is done */
    fout << ( ( N == 0 ) ? 0 : 1 ) << endl;
    return EXIT_SUCCESS;
  }

  return EXIT_FAILURE;
}
