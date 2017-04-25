/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#pragma once

#include <string>
#include <vector>
#include <cajun/json/elements.h>

class ThunkFunc
{
private:
  std::string exe_ {};
  std::vector<std::string> args_;
  std::string exe_hash_ {};

  void hash_exe();

  void parse_cmd();

public:
  ThunkFunc(std::vector<std::string> cmd);

  ~ThunkFunc();

  json::Object to_json();
};
