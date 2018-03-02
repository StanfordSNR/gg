/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <unistd.h>
#include <cstring>
#include <iostream>
#include <google/protobuf/text_format.h>

#include "thunk/thunk_reader.hh"
#include "thunk/thunk_writer.hh"
#include "thunk/thunk.hh"
#include "util/exception.hh"

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

    string exe = "gcc";
    vector<string> args = { "-DLOCALEDIR=\"/usr/local/share/locale\"",
      "-DLIBDIR=\"/usr/local/lib\"", "-DINCLUDEDIR=\"/usr/local/include\"",
      "-DHAVE_CONFIG_H", "-I.", "-g", "-O2", "-MT", "remake.o", "-MD", "-MP",
      "-MF", ".deps/remake.Tpo", "-c", "-o", "TEST_remake.o", "remake.i" };
    vector<string> envars = { "A=a", "B=b", "C=c" };

    Function function( exe, args, envars, "ABCDEFGHI" );

    InFile infile1( "thunk.hh", "thunk.hh", "XXX", 0, 120 );
    InFile infile2( "infile_desc.hh", "infile_desc.hh", "YYY", 0, 24000 );
    InFile infile3( "thunk_func.hh", "infile_desc.hh", "ZZZ", 0, 59200 );
    InFile indir1( "/home/sadjad/", "", InFile::Type::DUMMY_DIRECTORY );
    InFile indir2( "/etc/gg/", "", InFile::Type::DUMMY_DIRECTORY );

    vector<InFile> infiles = { infile1, infile2, infile3, indir1, indir2 };

    Thunk original_thunk { outfile, function, infiles };

    const string contents = ThunkWriter::serialize_thunk( original_thunk );
    roost::atomic_create( contents, outfile );

    // Now reading it back
    ThunkReader thunk_reader { outfile };
    Thunk thunk = thunk_reader.read_thunk();

    if ( thunk == original_thunk ) {
      return EXIT_SUCCESS;
    }
    else {
      return EXIT_FAILURE;
    }

  }
  catch ( const exception &  e ) {
    print_exception( argv[ 0 ], e );
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
