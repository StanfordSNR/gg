#pragma once

#include <string>

#include "cajun/json/elements.h"


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

    json::Object to_json();

};