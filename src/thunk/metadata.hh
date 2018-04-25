/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef THUNK_METADATA_HH
#define THUNK_METADATA_HH

#include <vector>
#include <string>

#include "thunk/factory.hh"

class PlaceholderMetadata
{
private:
  std::vector<std::string> args_ {};
  std::string cwd_ {};
  std::vector<ThunkFactory::Data> objects_ {};

public:
  PlaceholderMetadata( const std::vector<std::string> & args,
                       const std::string & cwd )
    : args_( args ), cwd_( cwd ) {}

  PlaceholderMetadata( const std::string & metadata_str );

  void add_object( const ThunkFactory::Data & object );
  std::string str() const;
};

#endif /* THUNK_METADATA_HH */
