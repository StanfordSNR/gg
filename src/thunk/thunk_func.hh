/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#pragma once

#include <string>
#include <vector>
#include <cajun/json/elements.h>

class ThunkFunc
{
private:
  std::string exe_ {};
  std::vector<std::string> args_; // args_ contains the exe_ in the first argument without the full path
  std::string exe_hash_ {};

  static std::string hash_exe( std::string exe );

  void parse_cmd();

  static std::string get_exe_path( std::string exe );

public:
  ThunkFunc( const std::vector<std::string> & cmd );

  ~ThunkFunc();

  json::Object to_json();
};
