/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#pragma once

#include <string>
#include <vector>

#include "gg.pb.h"
#include "thunk_func.hh"

class InFile
{
private:

  std::string filename_;
  std::string hash_;
  int order_;

  void is_thunk();
  void get_thunk_order();

  static std::string compute_hash( std::string filename );

public:
  // TODO : This currently does not check the order (if it is a thunk or not)
  InFile( std::string filename );
  InFile( const gg::protobuf::InFile & infile_proto );

  gg::protobuf::InFile to_protobuf() const;
};

class Thunk
{
private:
  std::string outfile_;
  ThunkFunc thunkfunc_;
  std::vector<InFile> infiles_;
  int order_; // TODO : check infiles to figure out order

public:
  Thunk( std::string outfile, ThunkFunc thunkfunc,
         std::vector<InFile> infiles );

  Thunk( const gg::protobuf::Thunk & thunk_proto );

  std::string outfile() const { return outfile_; }

  gg::protobuf::Thunk to_protobuf() const;
};
