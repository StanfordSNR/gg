/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <unistd.h>
#include <cstring>
#include <iostream>

#include "exception.hh"
#include "thunk.hh"
#include "thunk_reader.hh"
#include "child_process.hh"

using namespace std;
using namespace gg::thunk;

void usage( const char * argv0 )
{
  cerr << argv0 << " THUNK" << endl;
}

int main( int argc, char * argv[] )
{
  try {
    if ( argc <= 0 ) {
      abort();
    }

    if ( argc != 2 ) {
      usage( argv[ 0 ] );
      return EXIT_FAILURE;
    }

    ThunkReader thunk_reader { argv[ 1 ] };
    Thunk thunk = thunk_reader.read_thunk();

    ChildProcess process { thunk.outfile(), [&]() { return thunk.execute(); } };

    while ( not process.terminated() ) {
      process.wait();
    }

    cerr << "Exited: " << process.exit_status() << endl;
  }
  catch ( const exception &  e ) {
    print_exception( argv[ 0 ], e );
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
