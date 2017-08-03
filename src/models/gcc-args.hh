/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef GCC_ARGS_HH
#define GCC_ARGS_HH

#include <getopt.h>
#include <vector>
#include <string>
#include <map>

#include "optional.hh"

enum class GCCOption
{
  x = 1000,
  o,
  E, S, c,
  M, MD, MP, MT, MF,
  pie,
};

template <typename E>
constexpr auto to_underlying( E e ) noexcept
{
    return static_cast<std::underlying_type_t<E>>( e );
}

class GCCArguments
{
private:
  Optional<GCCStage> last_stage_ {};
  std::vector<InputFile> input_files_ {};

  std::map<GCCOption, std::pair<size_t, std::string>> opt_map_ {};
  std::vector<std::string> args_ {};
  std::vector<std::string> input_args_ {};
  std::string output_ {};

public:
  GCCArguments( const int argc, char ** argv );

  void add_option( const GCCOption option, const std::string & optstr,
                   const std::string & value = "" );

  void add_input( const std::string & filename, const Language language );
};

#endif /* GCC_ARGS_HH */
