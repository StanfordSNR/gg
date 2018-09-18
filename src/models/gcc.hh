/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef MODEL_LINKER_HH
#define MODEL_LINKER_HH

#include <string>
#include <vector>
#include <chrono>
#include <unordered_map>

#include "thunk/thunk.hh"
#include "thunk/factory.hh"
#include "thunk/ggutils.hh"
#include "util/optional.hh"
#include "util/path.hh"
#include "util/temp_file.hh"

#include "toolchain.hh"

enum GCCStage
{
  PREPROCESS = 1, COMPILE, ASSEMBLE, LINK
};

enum class Language
{
  NONE,
  C, C_HEADER, CPP_OUTPUT,
  CXX, CXX_HEADER, CXX_CPP_OUTPUT,
  ASSEMBLER, ASSEMBLER_WITH_CPP,
  OBJECT, SHARED_OBJECT, ARCHIVE_LIBRARY, SHARED_LIBRARY,
};

struct InputFile
{
  std::string name;
  Language language;
  Language source_language;
  size_t index;

  ThunkFactory::Data indata {};
};

enum class OperationMode
{
  GCC, GXX
};

enum class GCCOption
{
  c = 1000, C, D, dashstatic, dM, dumpmachine, dumpversion, E, f, g, g0, g1,
  gdwarf_4, include, isystem, M, m32, m64, march, mavx, mavx2, mbmi, mbmi2,
  mcmodel, MD, MF, mf16c, mfentry, mfma, MMD, mmmx, mno_3dnow, mno_80387,
  mno_avx, mno_fp_ret_in_387, mno_mmx, mno_red_zone, mno_sse, mno_sse2,
  mno_tls_direct_seg_refs, MP, mpclmul, mpreferred_stack_boundary, mregparm,
  mskip_rax_setup, msoft_float, msse, msse2, msse4_1, msse4_2, mssse3, MT, mtune,
  nodefaultlibs, nostdinc, nostdincpp, nostdlib, o, O, P, param, pedantic, pg,
  pie, pipe, print_file_name, pthread, rdynamic, s, S, shared, std, u, U, v, w,
  x, Xlinker
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
static const roost::path GG_SYSROOT_PREFIX = "./__gg_sysroot__";

static constexpr char DEPGEN_WORKING_DIRECTORY[] = "_GG_WORKING_DIRECTORY";
static constexpr char DEPGEN_INCLUDE_TARBALLS[]  = "_GG_INCLUDE_TARBALLS";
static constexpr char DEPGEN_INPUT_NAME[]        = "_GG_INPUT_NAME";
static constexpr char DEPGEN_TARGET_NAME[]       = "_GG_TARGET_NAME";
static constexpr char DEPGEN_GCC_HASH[]          = "_GG_GCC_HASH";
static constexpr char DEPGEN_CC1_NAME[]          = "_GG_CC1_NAME";
static constexpr char DEPGEN_CC1_HASH[]          = "_GG_CC1_HASH";

class GCCArguments
{
private:
  Optional<GCCStage> last_stage_ {};
  std::vector<InputFile> input_files_ {};

  std::map<GCCOption, std::pair<size_t, std::string>> opt_map_ {};
  std::vector<std::string> args_ {};
  std::string output_ {};

  std::vector<std::string> include_dirs_ {};
  std::vector<std::string> library_dirs_ {};
  std::vector<std::string> system_include_dirs_ {};

  std::unordered_map<GCCStage, std::vector<std::string>> extra_infiles_ {};

  bool no_stdlib_ { false };
  bool no_stdinc_ { false };
  bool no_stdincpp_ { false };
  bool no_defaultlibs_ { false };

  const bool force_strip_ { false };
  const bool canonical_paths_ { false };
  roost::path canonical_root_ {};

  std::string path_fn( const std::string & path );

public:
  GCCArguments( const int argc, char * const * argv,
                const bool force_strip = false,
                const bool canonical_paths = false );

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
  const std::vector<std::string> & system_include_dirs() const { return system_include_dirs_; }
  const std::vector<std::string> & library_dirs() const { return library_dirs_; }
  const std::vector<std::string> & extra_infiles( const GCCStage stage );
  bool no_stdlib() const { return no_stdlib_; }
  bool no_stdinc() const { return no_stdinc_; }
  bool no_stdincpp() const { return no_stdincpp_; }
  bool no_defaultlibs() const { return no_defaultlibs_; }

  Optional<std::string> option_argument( const GCCOption option ) const;
  const std::vector<std::string> all() const { return args_; }

  bool force_strip() const { return force_strip_; }
};

class GCCModelGenerator
{
private:
  std::function<void(void)> exec_original_gcc {};

  OperationMode operation_mode_;
  GCCArguments arguments_;
  TempFile specs_tempfile_ { "/tmp/gg-gccspecs" };

  const bool preprocess_locally_ { false };
  const bool all_in_one_thunk_ { false };
  const bool defer_depgen_ { false };

  std::vector<std::string> envars_ { { "PATH=" + GG_BIN_PREFIX }, };

  std::vector<std::string> get_link_dependencies( const std::vector<InputFile> & link_inputs );

  std::string do_preprocessing( const InputFile & input );

  std::string generate_thunk( const GCCStage first_stage,
                              const GCCStage stage,
                              const InputFile & input,
                              const std::string & output,
                              const bool write_placeholder );

  std::string generate_link_thunk( const std::vector<InputFile> & link_inputs,
                                   const std::vector<std::string> & dependencies,
                                   const std::string & output );

  static std::vector<std::string> gcc_environment();

  std::vector<roost::path> scan_build_directory( const roost::path & build_dir,
                                                 const std::vector<roost::path> & excludes ) const;

public:
  struct Options
  {
    static constexpr int preprocess_locally = ( 1 << 0 );
    static constexpr int all_in_one_thunk   = ( 1 << 1 );
    static constexpr int defer_depgen       = ( 1 << 2 );
  };

  GCCModelGenerator( const OperationMode operation_mode,
                     int argc, char ** argv, const int options );
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

  static std::vector<std::string>
  parse_dependencies_file( const std::string & dep_filename,
                           const std::string & target_name );

  static std::vector<std::string>
  generate_dependencies_file( const OperationMode op_mode,
                              const std::string & input_filename,
                              const std::vector<std::string> & option_args,
                              const std::string & output_name,
                              const std::string & target_name,
                              const bool use_cache = true,
                              const std::string & gcc_binary = {},
                              const Optional<std::vector<std::string>> & env = {} );
};

#endif /* MODEL_LINKER_HH */
