#pragma once

#include <string>
#include <vector>

#include "json.hh"

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