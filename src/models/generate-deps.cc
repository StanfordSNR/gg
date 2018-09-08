/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <iostream>

#include "gcc.hh"
#include "thunk/thunk_reader.hh"
#include "thunk/thunk.hh"
#include "util/exception.hh"
#include "util/system_runner.hh"

using namespace std;
using namespace gg::thunk;

void usage( const char * argv0 )
{
  cerr << argv0 << " PREPROCESSOR-THUNK" << endl;
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

    Thunk thunk = ThunkReader::read( argv[ 1 ] );

    vector<string> thunk_args = thunk.function().args();
    const string program_name = thunk_args[ 0 ];

    const OperationMode op_mode = ( thunk_args[ 0 ] == program_data.at( GCC ).filename() )
                                  ? OperationMode::GCC : OperationMode::GXX;

    thunk_args.pop_back();
    thunk_args.pop_back();
    thunk_args.erase( thunk_args.begin(), thunk_args.begin() + 2 );

    GCCModelGenerator::generate_dependencies_file( op_mode, "/dev/null",
                                                   thunk_args, "deps", "SADJAD" );
  }
  catch ( const exception &  e ) {
    print_exception( argv[ 0 ], e );
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
