/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#pragma once

#include <string>
#include <cajun/json/elements.h>

class InFileDescriptor
{
private:
  std::string hash_;
  std::string filename_;
  int order_;

  void compute_hash();

  void is_thunk();

  void get_thunk_order();

public:
  InFileDescriptor( std::string filename );

  ~InFileDescriptor();

  json::Object to_json();
};
