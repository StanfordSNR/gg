#pragma once

#include <string>
#include <vector>

#include "cajun/json/elements.h"

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

    json::Object to_json();
};