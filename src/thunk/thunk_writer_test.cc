#include <iostream>
#include <string>
#include <vector>

#include "thunk_func.hh"
#include "infile_desc.hh"
#include "thunk.hh"
#include "thunk_writer.hh"

#include "cajun/json/reader.h"
#include "cajun/json/writer.h"
#include "cajun/json/elements.h"

using namespace std;



int main() {
    cout << "Test started ... " << endl;

    string outfile = "remake.o";

    vector<string> cmd = {"gcc", "-DLOCALEDIR=\"/usr/local/share/locale\"",
        "-DLIBDIR=\"/usr/local/lib\"", "-DINCLUDEDIR=\"/usr/local/include\"",
        "-DHAVE_CONFIG_H", "-I.", "-g", "-O2", "-MT", "remake.o", "-MD", "-MP",
        "-MF", ".deps/remake.Tpo", "-c", "-o", "remake.o", "remake.i"};

    ThunkFunc thunkfunc(cmd);

    InFileDescriptor infile1("job.h");
    InFileDescriptor infile2("commands.h");
    InFileDescriptor infile3("dep.h");

    vector<InFileDescriptor> infiles = {infile1, infile2, infile3};

    Thunk thunk(outfile, thunkfunc, infiles);

    ThunkWriter::write_thunk(thunk);

}