/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#pragma once

#include <string>
#include <vector>
#include <cajun/json/elements.h>
#include <cajun/json/writer.h>

#include "thunk_func.hh"
#include "infile_desc.hh"

class Thunk
{
private:
  std::string outfile_;
  ThunkFunc thunkfunc_;
  std::vector<InFileDescriptor> infiles_;
  int order_; // TODO : check infiles to figure out order

public:
  Thunk( std::string outfile, ThunkFunc thunkfunc,
         std::vector<InFileDescriptor> infiles );

  ~Thunk();

  std::string get_outfile();

  json::Object to_json();
};
