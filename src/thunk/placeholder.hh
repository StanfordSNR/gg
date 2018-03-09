/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef PLACEHOLDER_HH
#define PLACEHOLDER_HH

#include <sys/types.h>
#include <string>

#include "util/optional.hh"
#include "util/file_descriptor.hh"

class ThunkPlaceholder
{
private:
  std::string content_hash_;

public:
  enum class Type { LinkerScript, ShellScript };

  ThunkPlaceholder( const std::string & content_hash );

  std::string content_hash() const { return content_hash_; }

  void write( const std::string & filename, const Type type ) const;
  void write( const std::string & filename ) const;
  static Optional<ThunkPlaceholder> read( const std::string & filename );

  static bool is_placeholder( FileDescriptor && fd );
};

#endif /* PLACEHOLDER_HH */
