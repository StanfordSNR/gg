#pragma once

#include <string>
#include <vector>
#include "thunk.hh"


using namespace std;


class ThunkWriter
{
private:
    ThunkWriter();

public:

    static bool write_thunk(Thunk thunk);

    static Thunk read_thunk( string filename );
};