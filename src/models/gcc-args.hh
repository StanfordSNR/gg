/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef GCC_ARGS_HH
#define GCC_ARGS_HH

enum class GCCOption
{
  x = 1000,
  o,
  E, S, c,
  M, MD, MP, MT, MF,
  pie,
};

class GCCArguments
{
public:
  GCCArguments( const int argc, const char * argv );
};

#endif /* GCC_ARGS_HH */
