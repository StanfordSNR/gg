#pragma once

#include <string>
#include <vector>
#include "thunk_func.hh"
#include "infile_desc.hh"

#include "json.hh"

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

    Json::Value to_json();

};