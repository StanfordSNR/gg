/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#pragma once

#include <string>
#include <vector>

#include "gg.pb.h"
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

  Thunk( const gg::protobuf::Thunk & thunk_proto );

  std::string outfile() const { return outfile_; }

  gg::protobuf::Thunk to_protobuf() const;
};
