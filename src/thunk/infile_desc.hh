#pragma once

#include <string>

#include "json.hh"

using namespace std;


class InFileDescriptor
{
private:
    string hash;
    string filename;
    int order;

    void computeHash();

    void isThunk();

    void getThunkOrder();

public:
    InFileDescriptor( string filename );

    ~InFileDescriptor();

    Json::Value to_json();

};