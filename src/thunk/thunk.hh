#pragma once

#include <string>
#include <vector>
#include "thunk_func.hh"
#include "infile_desc.hh"

#include "cajun/json/elements.h"
#include "cajun/json/writer.h"


class Thunk
{
private:
    std::string outfile_;
    ThunkFunc thunkfunc_;
    std::vector<InFileDescriptor> infiles_;
    int order_; // TODO : check infiles to figure out order

public:
    Thunk( std::string outfile,  ThunkFunc thunkfunc, std::vector<InFileDescriptor> infiles );

    ~Thunk();

    json::Object to_json();

};