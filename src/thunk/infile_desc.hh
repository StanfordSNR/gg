#pragma once

#include <string>

#include "json.hpp"

#ifdef DANITER_CLION
#include "../../third-party/json/src/json.hpp"
#endif

using json = nlohmann::json;

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

    json to_json();

};