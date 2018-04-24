/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>

#include "thunk/thunk.hh"
#include "thunk/thunk_writer.hh"
#include "util/exception.hh"
#include "util/path.hh"
#include "util/util.hh"

using namespace std;
using namespace gg;
using namespace gg::thunk;

const string dummy_exe_hash = "VhhJgsB0p6m7sEjxulM1DDsPe9nE.7ES2L37n.8uEY5w004a4ae0";
const string dummy_inf_hash = "VhnrdEwX3skW5tIYDaLnHu.vHFZhpjsaY0s13kJl4UbI001b4b88";

int main( int argc, char * argv[] )
{
  try {
    if ( argc != 2 ) {
      cerr << "usage: test_thunk_gen <num-thunks>" << endl;
      return EXIT_FAILURE;
    }

    const long long N = stoll( argv[ 1 ] );

    if ( N < 0 ) {
      cerr << argv[ 0 ] << " doesn't accept negative inputs" << endl;
      return EXIT_FAILURE;
    }

    for ( int i = 0; i < N; ++i ) {
      const Thunk next_thunk {
        { dummy_exe_hash, 
          { "my_func", "param1", "thunknum" + to_string( i ) }, {} },
        { { dummy_inf_hash, "" } },
        { { dummy_exe_hash, " " } },
        { { "OUTPUT" + to_string( i ) } }
      };
      
      ThunkWriter::write( next_thunk, "OUTPUT" + to_string( i ) );
    }

    cout << "Done generating " << N << " thunks" << endl;
  }
  catch ( const exception &  e ) {
    print_exception( argv[ 0 ], e );
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
