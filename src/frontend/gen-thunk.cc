/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <unistd.h>
#include <cstring>
#include <iostream>

#include "exception.hh"
#include "thunk.hh"
#include "thunk_reader.hh"
#include "thunk_writer.hh"

using namespace std;
using namespace google::protobuf;
using namespace gg::thunk;

void usage( const char * argv0 )
{
  cerr << argv0 << endl;
}

int main( int argc, char * argv[] )
{
  try {
    if ( argc <= 0 ) {
      abort();
    }

    if ( argc != 1 ) {
      usage( argv[ 0 ] );
      return EXIT_FAILURE;
    }

    // First writing out a thunk
    string outfile = "TEST_remake.o";

    vector<string> cmd = { "gcc", "-DLOCALEDIR=\"/usr/local/share/locale\"",
      "-DLIBDIR=\"/usr/local/lib\"", "-DINCLUDEDIR=\"/usr/local/include\"",
      "-DHAVE_CONFIG_H", "-I.", "-g", "-O2", "-MT", "remake.o", "-MD", "-MP",
      "-MF", ".deps/remake.Tpo", "-c", "-o", "TEST_remake.o", "remake.i" };

    Function Function( cmd );

    InFile infile1( "thunk.hh", "XXX", 0 );
    InFile infile2( "infile_desc.hh", "YYY", 0 );
    InFile infile3( "thunk_func.hh", "ZZZ", 0 );

    vector<InFile> infiles = { infile1, infile2, infile3 };

    Thunk original_thunk { outfile, Function, infiles };

    ThunkWriter::write_thunk( original_thunk );
  }
  catch ( const exception &  e ) {
    print_exception( argv[ 0 ], e );
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
