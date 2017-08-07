/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef MODEL_LINKER_HH
#define MODEL_LINKER_HH

#include <string>
#include <vector>
#include <unordered_map>

#include "thunk.hh"
#include "toolchain.hh"
#include "gcc-args.hh"
#include "optional.hh"
#include "temp_file.hh"

enum GCCStage
{
  PREPROCESS = 1,
  COMPILE,
  ASSEMBLE,
  LINK
};

enum class Language
{
  NONE,
  C,
  C_HEADER,
  CPP_OUTPUT,
  CXX,
  CXX_HEADER,
  CXX_CPP_OUTPUT,
  ASSEMBLER,
  OBJECT,
  ARCHIVE_LIBRARY,
  SHARED_LIBRARY,
};

struct InputFile
{
  std::string name;
  Language language;
  Language source_language;
  size_t index;
};

enum class OperationMode
{
  GCC,
  GXX,
};

enum class GCCOption
{
  x = 1000,
  o,
  E, S, c,
  M, MD, MP, MT, MF,
  pie,
  include,
  pthread, shared,
  param, pipe
};

#define PROGRAM(x) \
  { x, { GG_BIN_PREFIX + "/" + x, ( toolchain_path / x ).string(), program_hash( x ), 0 } }

static const std::string GCC { "gcc" };
static const std::string GXX { "g++" };
static const std::string AS  { "as" };
static const std::string CC1 { "cc1" };
static const std::string CC1PLUS { "cc1plus" };
static const std::string COLLECT2 { "collect2" };
static const std::string LD { "ld" };
static const std::string GG_BIN_PREFIX { "/__gg__" };
static const roost::path toolchain_path { std::string( TOOLCHAIN_PATH ) };

static auto gcc_function =
  []( const OperationMode operation_mode,
      const std::vector<std::string> & args,
      const std::vector<std::string> & envars ) -> gg::thunk::Function
  {
    switch ( operation_mode ) {
    case OperationMode::GCC:
      return { GG_BIN_PREFIX + "/" + GCC, args, envars, program_hash( GCC ) };

    case OperationMode::GXX:
      return { GG_BIN_PREFIX + "/" + GXX, args, envars, program_hash( GXX ) };

    default:
      throw std::runtime_error( "invalid operation mode" );
    }
  };

static const std::unordered_map<std::string, gg::thunk::InFile> program_infiles {
  PROGRAM( GCC ), PROGRAM( GXX ), PROGRAM( CC1 ), PROGRAM( CC1PLUS ),
  PROGRAM( AS ), PROGRAM( COLLECT2 ), PROGRAM( LD ),
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
  const std::vector<InputFile> & input_files() const { return input_files_; }
  GCCStage last_stage() const { return last_stage_.get_or( LINK ); }
  const std::vector<std::string> & include_dirs() const { return include_dirs_; }

  const std::vector<std::string> & option_args() const { return args_; }
  std::vector<std::string> all_args() const;

  Optional<std::string> option_argument( const GCCOption option ) const;

  void print_args() const;
};

class GCCModelGenerator
{
private:
  OperationMode operation_mode_;
  GCCArguments arguments_;
  TempFile specs_tempfile_ { "/tmp/gg-gccspecs" };

  std::vector<std::string> envars_ { { "PATH=" + GG_BIN_PREFIX }, };

  std::vector<std::string> get_link_dependencies( const std::vector<InputFile> & link_inputs,
                                                  const std::vector<std::string> & args );

  std::vector<std::string> parse_dependencies_file( const std::string & dep_filename,
                                                    const std::string & target_name );

  void generate_dependencies_file( const std::vector<std::string> & option_args,
                                   const std::string & input_name,
                                   const std::string & output_name );

  gg::thunk::Thunk generate_thunk( const GCCStage stage,
                                   const InputFile & input,
                                   const std::string & output );

  gg::thunk::Thunk generate_link_thunk( const std::vector<InputFile> & link_inputs,
                                        const std::vector<std::string> & link_args,
                                        const std::vector<std::string> & dependencies,
                                        const std::string & output );

public:
  GCCModelGenerator( const OperationMode operation_mode, int argc, char ** argv );
  void generate();

  /* static functions */
  static Language    filename_to_language( const std::string & path );
  static Language    name_to_language( const std::string & name );
  static GCCStage    language_to_stage( const Language lang );
  static std::string language_to_name( const Language & lang );
  static std::string stage_output_name( const GCCStage stage, const std::string basename );
};

#endif /* MODEL_LINKER_HH */
