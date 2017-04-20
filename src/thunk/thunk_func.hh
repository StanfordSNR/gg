#pragma once

#include <string>
#include <vector>

#include "json.hpp"

#ifdef DANITER_CLION
#include "../../third-party/json/src/json.hpp"
#endif

using json = nlohmann::json;

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

    json to_json();
};