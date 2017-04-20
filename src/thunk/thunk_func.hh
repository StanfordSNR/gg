#pragma once

#include <string>
#include <vector>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"
#include <json/json.h>
#pragma GCC diagnostic pop

using namespace std;


class ThunkFunc
{
    string exe = "";
    vector<string> args;
    string exe_hash = "";

    void hashExe();

    void parseCmd();

public:
    ThunkFunc(vector<string> cmd);

    ~ThunkFunc();

    Json::Value to_json();
};