/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef MODEL_LINKER_HH
#define MODEL_LINKER_HH

#include <string>
#include <vector>
#include <unordered_map>

#include "thunk/factory.hh"
#include "thunk/thunk.hh"
#include "util/optional.hh"
#include "util/temp_file.hh"

#include "toolchain.hh"

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
  ASSEMBLER_WITH_CPP,
  OBJECT,
  SHARED_OBJECT,
  ARCHIVE_LIBRARY,
  SHARED_LIBRARY,
};

struct InputFile
{
  std::string name;
  Language language;
  Language source_language;
  size_t index;

  ThunkFactory::Data indata;
};

enum class OperationMode
{
  GCC,
  GXX,
};

enum class GCCOption
{
  x = 1000,
  g, O, D, U, f, w, v,
  o,
  E, S, c, C,
  M, MD, MP, MT, MF, MMD,
  pie,
  include, isystem,
  pthread, shared,
  param, pipe, pedantic, dashstatic, s,
  nostdlib, nostdinc, std,
  Xlinker, mcmodel, mfentry, gdwarf_4, pg, P,

  mno_sse, mno_mmx, mno_sse2, mno_3dnow, mno_avx, m32, m64, mno_80387,
  mno_fp_ret_in_387, mno_red_zone, msoft_float, mregparm,
  msse4_2,
  mno_tls_direct_seg_refs,

  mpreferred_stack_boundary,
  mskip_rax_setup, mtune, march,

  print_file_name,

  dM, rdynamic, dumpversion, dumpmachine
};

struct GCCOptionData;

static auto gcc_function =
  []( const OperationMode operation_mode,
      const std::vector<std::string> & args,
      const std::vector<std::string> & envars ) -> gg::thunk::Function
  {
    switch ( operation_mode ) {
    case OperationMode::GCC:
      return { program_hash( GCC ), args, envars,  };

    case OperationMode::GXX:
      return { program_hash( GXX ), args, envars };

    default:
      throw std::runtime_error( "invalid operation mode" );
    }
  };

static const std::string DEFAULT_MAKE_TARGET = "__GG_MAKE_TARGET__";

class GCCArguments
{
private:
  Optional<GCCStage> last_stage_ {};
  std::vector<InputFile> input_files_ {};

  std::map<GCCOption, std::pair<size_t, std::string>> opt_map_ {};
  std::vector<std::string> args_ {};
  std::vector<std::string> input_args_ {};
  std::string output_ {};

  std::vector<std::string> include_dirs_ {};
  std::vector<std::string> library_dirs_ {};
  std::vector<std::string> system_include_dirs_ {};

  std::unordered_map<GCCStage, std::vector<std::string>> extra_infiles_ {};

  bool no_stdlib_ { false };
  bool no_stdinc_ { false };

  const bool force_strip_ { false };

public:
  GCCArguments( const int argc, char ** argv, const bool force_strip = false );

  void add_option( const GCCOption option, const std::string & optstr,
                   const char * optargx = nullptr, const char arg_separator = 'X',
                   const bool double_dash = false );

  void add_option( const GCCOptionData & option_data, const char * optarg );

  void add_input( const std::string & filename, const Language language );

  void process_W_option( const std::string & optarg );

  const std::string & output_filename() const { return output_; }
  const std::vector<InputFile> & input_files() const { return input_files_; }
  GCCStage last_stage() const { return last_stage_.get_or( LINK ); }
  const std::vector<std::string> & include_dirs() const { return include_dirs_; }
  const std::vector<std::string> & library_dirs() const { return library_dirs_; }
  const std::vector<std::string> & extra_infiles( const GCCStage stage );
  bool no_stdlib() const { return no_stdlib_; }
  bool no_stdinc() const { return no_stdinc_; }

  const std::vector<std::string> & option_args() const { return args_; }
  std::vector<std::string> all_args() const;

  Optional<std::string> option_argument( const GCCOption option ) const;

  void print_args() const;

  bool force_strip() const { return force_strip_; }
};

class GCCModelGenerator
{
private:
  std::function<void(void)> exec_original_gcc {};

  OperationMode operation_mode_;
  GCCArguments arguments_;
  TempFile specs_tempfile_ { "/tmp/gg-gccspecs" };

  std::vector<std::string> envars_ { { "PATH=" + GG_BIN_PREFIX }, };

  std::vector<std::string> get_link_dependencies( const std::vector<InputFile> & link_inputs,
                                                  const std::vector<std::string> & args );

  std::vector<std::string> parse_dependencies_file( const std::string & dep_filename,
                                                    const std::string & target_name );

  std::vector<std::string> generate_dependencies_file( const std::vector<std::string> & option_args,
                                                       const std::string & output_name,
                                                       const std::string & target_name );

  std::string generate_thunk( const GCCStage first_stage,
                              const GCCStage stage,
                              const InputFile & input,
                              const std::string & output,
                              const bool write_placeholder );

  std::string generate_link_thunk( const std::vector<InputFile> & link_inputs,
                                   const std::vector<std::string> & dependencies,
                                   const std::string & output );

  static std::vector<std::string> gcc_environment();

public:
  GCCModelGenerator( const OperationMode operation_mode, int argc, char ** argv );
  void generate();

  /* static functions */
  static std::pair<std::string, std::string> split_source_name( const std::string & path );
  static Language    filename_to_language( const std::string & path );
  static Language    name_to_language( const std::string & name );
  static GCCStage    language_to_stage( const Language lang );
  static std::string language_to_name( const Language & lang );
  static std::string stage_output_name( const GCCStage stage, const std::string basename );

  static std::vector<std::string>
  parse_linker_output( const std::vector<std::string> & linker_args );
};

#endif /* MODEL_LINKER_HH */
