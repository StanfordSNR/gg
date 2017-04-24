#pragma once

#include <string>
#include <vector>

#include "cajun/json/elements.h"

class ThunkFunc
{
    std::string exe_ = "";
    std::vector<std::string> args_;
    std::string exe_hash_ = "";

    void hash_exe();

    void parse_cmd();

public:
    ThunkFunc(std::vector<std::string> cmd);

    ~ThunkFunc();

    json::Object to_json();
};