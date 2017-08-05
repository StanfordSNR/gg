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
  include,
  pthread, shared,
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

  std::vector<std::string> include_dirs_{};

public:
  GCCArguments( const int argc, char ** argv );

  void add_option( const GCCOption option, const std::string & optstr,
                   const std::string & value = "" );

  void add_input( const std::string & filename, const Language language );

  const std::string & output_filename() const { return output_; }
  const std::vector<InputFile> input_files() const { return input_files_; }
  GCCStage last_stage() const { return last_stage_.get_or( LINK ); }
  const std::vector<std::string> & include_dirs() const { return include_dirs_; }

  const std::vector<std::string> & option_args() const { return args_; }
  std::vector<std::string> all_args() const;

  Optional<std::string> option_argument( const GCCOption option ) const;

  void print_args() const;
};

#endif /* GCC_ARGS_HH */
