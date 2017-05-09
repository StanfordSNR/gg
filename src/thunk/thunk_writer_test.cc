/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <iostream>
#include <string>
#include <vector>

#include "thunk_func.hh"
#include "thunk.hh"
#include "thunk_writer.hh"

using namespace std;

int main() {
  cout << "Test started ... " << endl;

  string outfile = "remake.o";

  vector<string> cmd = { "gcc", "-DLOCALEDIR=\"/usr/local/share/locale\"",
    "-DLIBDIR=\"/usr/local/lib\"", "-DINCLUDEDIR=\"/usr/local/include\"",
    "-DHAVE_CONFIG_H", "-I.", "-g", "-O2", "-MT", "remake.o", "-MD", "-MP",
    "-MF", ".deps/remake.Tpo", "-c", "-o", "remake.o", "remake.i" };

  ThunkFunc thunkfunc( cmd );

  InFile infile1( "thunk.hh" );
  InFile infile2( "infile_desc.hh" );
  InFile infile3( "thunk_func.hh" );

  vector<InFile> infiles = { infile1, infile2, infile3 };

  Thunk thunk( outfile, thunkfunc, infiles );

  ThunkWriter::write_thunk( thunk );
}
