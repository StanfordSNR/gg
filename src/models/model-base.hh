/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#pragma once

#include <string.h>
#include <vector>

#include "thunk.hh"

class ModelBase
{
private:
  void copy_infiles_to_gg( std::vector<gg::thunk::InFile> & infiles );

protected:
  const std::string GG_DIR;
  static const std::string GG_DIR_FLAG;

  std::string srcfile {};
  std::string outfile {};
  std::vector<std::string> cmd {};

  void parse_args( int, char** );
  void store_args( int, char** );

  gg::thunk::Function get_function();
  std::string get_outfile();

  virtual std::vector<gg::thunk::InFile> get_infiles() = 0;

public:
  ModelBase( int, char** );
  ModelBase( const std::vector<std::string> & args );
  virtual ~ModelBase() = 0;

  std::string get_srcfile( int, char ** );
  gg::thunk::Thunk build_thunk();
  void write_thunk();
};
