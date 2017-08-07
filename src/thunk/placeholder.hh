/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef PLACEHOLDER_HH
#define PLACEHOLDER_HH

#include <string>

#include "optional.hh"

class ThunkPlaceholder
{
private:
  std::string hash_;
  bool execute_after_force_;

public:
  ThunkPlaceholder( const std::string & hash, const bool execute_after_force );

  std::string hash() const { return hash_; }

  void write( const std::string & filename ) const;
  static Optional<ThunkPlaceholder> read( const std::string & filename );
};

#endif /* PLACEHOLDER_HH */
