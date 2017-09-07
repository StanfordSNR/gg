/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef PLACEHOLDER_HH
#define PLACEHOLDER_HH

#include <sys/types.h>
#include <string>

#include "optional.hh"
#include "file_descriptor.hh"

static const std::string SHEBANG_DIRECTIVE { "#!/usr/bin/env gg-reduce-and-run" };

class ThunkPlaceholder
{
private:
  std::string content_hash_;
  size_t order_;
  off_t size_;

public:
  ThunkPlaceholder( const std::string & content_hash,
                    const size_t order,
                    const off_t size );

  std::string content_hash() const { return content_hash_; }
  size_t order() const { return order_; }
  off_t size() const { return size_; }

  void write( const std::string & filename ) const;
  static Optional<ThunkPlaceholder> read( const std::string & filename );

  static bool is_placeholder( FileDescriptor && fd );
};

#endif /* PLACEHOLDER_HH */
