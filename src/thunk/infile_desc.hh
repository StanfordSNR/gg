#pragma once

#include <string>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"
#include <json/json.h>
#pragma GCC diagnostic pop

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