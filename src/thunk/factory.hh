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
  struct Options
  {
    static constexpr int generate_manifest  = ( 1 << 0 );
    static constexpr int create_placeholder = ( 1 << 1 );
    static constexpr int collect_data       = ( 1 << 2 );
  };

  class Data
  {
  private:
    std::string filename_ {};
    std::string real_filename_ {};
    std::string hash_ {};
    gg::ObjectType type_ { gg::ObjectType::Value };

  public:
    Data() {}

    Data( const std::string & filename,
          const std::string & real_filename = {},
          const gg::ObjectType & type = gg::ObjectType::Value,
          const std::string & hash = {} );

    const std::string & filename() const { return filename_; }
    const std::string & real_filename() const { return real_filename_; }
    const std::string & hash() const { return hash_; }
    gg::ObjectType type() const { return type_; }

    static std::string compute_hash( const std::string & real_filename,
                                     const gg::ObjectType type = gg::ObjectType::Value );
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
                               const std::vector<Data> & executables,
                               const std::vector<Output> & outputs,
                               const std::vector<std::string> & dummy_dirs = {},
                               const int options = Options::create_placeholder | Options::collect_data );
};

#endif /* FACTORY_HH */
