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

int main( int argc, char * argv[] )
{
  try {
    if ( argc != 2 ) {
      cerr << "usage: fib <N>" << endl;
      return EXIT_FAILURE;
    }

    const long long N = stoll( argv[ 1 ] );

    if ( N < 0 ) {
      cerr << argv[ 0 ] << " doesn't accept negative inputs" << endl;
      return EXIT_FAILURE;
    }

    if ( N < 2 ) {
      /* in this case, we just return the value and our job is done */
      ofstream fout { "out" };
      fout << ( ( N == 0 ) ? 0 : 1 ) << endl;
      roost::atomic_create( "", "left" );
      roost::atomic_create( "", "right" );
      return EXIT_SUCCESS;
    }

    const string fib_func_hash = safe_getenv( "FIB_FUNCTION_HASH" );
    const string add_func_hash = safe_getenv( "ADD_FUNCTION_HASH" );

    vector<string> envars = { "FIB_FUNCTION_HASH=" + fib_func_hash,
                              "ADD_FUNCTION_HASH=" + add_func_hash };

    const Thunk fib_left {
      { fib_func_hash, { "fib", to_string( N - 1 ) }, envars },
      {},
      { { fib_func_hash, "" } },
      { { "out" }, { "left" }, { "right" } }
    };

    const Thunk fib_right {
      { fib_func_hash, { "fib", to_string( N - 2 ) }, envars },
      {},
      { { fib_func_hash, "" } },
      { { "out" }, { "left" }, { "right" } }
    };

    const string fib_left_hash = ThunkWriter::write( fib_left, "left" );
    const string fib_right_hash = ThunkWriter::write( fib_right, "right" );

    const Thunk add_thunk {
      { add_func_hash, { "add",
                         thunk::data_placeholder( fib_left_hash ),
                         thunk::data_placeholder( fib_right_hash ) }, {} },
      { { fib_left_hash, "" }, { fib_right_hash, "" } },
      { { add_func_hash, "" } },
      { { "out" } }
    };

    ThunkWriter::write( add_thunk, "out" );
  }
  catch ( const exception &  e ) {
    print_exception( argv[ 0 ], e );
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
