#pragma once

#include <string>
#include <vector>
#include "thunk_func.hh"
#include "infile_desc.hh"

#include "cajun/json/elements.h"
#include "cajun/json/writer.h"


using namespace std;


class Thunk
{
private:
    string outfile;
    ThunkFunc thunkfunc;
    vector<InFileDescriptor> infiles;
    int order; // TODO : check infiles to figure out order

public:
    Thunk( string outfile,  ThunkFunc thunkfunc, vector<InFileDescriptor> infiles);

    ~Thunk();

    json::Object to_json();

};