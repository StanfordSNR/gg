/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef UTIL_ARGS_HH
#define UTIL_ARGS_HH

#include <vector>

struct ExpandedArgs
{
  std::vector<char *> args {};
  std::vector<std::vector<char>> arg_data {};

  ExpandedArgs() {}

  static bool needs_expansion( const int argc, char * const * argv );
  static ExpandedArgs expand( const int argc, char * const * argv );
};

#endif /* UTIL_ARGS_HH */
