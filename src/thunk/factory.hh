/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef FACTORY_HH
#define FACTORY_HH

#include <string>
#include <vector>
#include <sys/types.h>

#include "thunk.hh"

class ThunkFactory
{
public:
  class Data
  {
  private:
    std::string filename_;
    std::string real_filename_;
    std::string hash_;
    gg::ObjectType type_;

    std::string compute_hash() const;

  public:
    Data( const std::string & filename,
          const std::string & real_filename = {},
          const gg::ObjectType & type = gg::ObjectType::Value,
          const std::string & hash = {} );

    const std::string & filename() const { return filename_; }
    const std::string & real_filename() const { return real_filename_; }
    const std::string & hash() const { return hash_; }
    gg::ObjectType type() const { return type_; }
  };

  using Function = gg::thunk::Function;

  class Output
  {
  private:
    const std::string tag_;
    Optional<std::string> filename_ {};

  public:
    Output( const std::string & tag ) : tag_( tag ) {}
    Output( const std::string & tag, const std::string & filename )
      : tag_( tag ), filename_( true, filename ) {}

    const std::string & tag() const { return tag_; }
    const Optional<std::string> & filename() const { return filename_; }
  };

public:
  static std::string generate( const Function & function,
                               const std::vector<Data> & data,
                               const std::vector<Output> & outputs,
                               const bool generate_manifest = false,
                               const std::vector<std::string> & dummy_dirs = {},
                               const bool create_placeholder = true,
                               const bool collect_data = true );
};

#endif /* FACTORY_HH */
