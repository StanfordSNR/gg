/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "model-gcc.hh"

#include <iostream>
#include <getopt.h>

#include "util/util.hh"

using namespace std;

struct GCCOptionData
{
  GCCOption option;
  const char * option_str;
  int has_arg;
  bool double_dash;
  char arg_separator;
};

bool startswith( const string & str, const string & prefix ) {
  return str.compare( 0, prefix.size(), prefix ) == 0;
}

template<size_t N>
struct getopt_options
{
  constexpr getopt_options( const GCCOptionData gcc_options_data[] )
    : list()
  {
    size_t i = 0;
    for ( ; i < N; i++ ) {
      list[ i ].name = gcc_options_data[ i ].option_str;
      list[ i ].has_arg = gcc_options_data[ i ].has_arg;
      list[ i ].flag = nullptr;
      list[ i ].val = to_underlying( gcc_options_data[ i ].option );
    }

    list[ i ] = { nullptr, 0, nullptr, 0 };
  }

  option list[ N + 1 ];
};

GCCArguments::GCCArguments( const int argc, char ** argv, const bool force_strip )
  : force_strip_( force_strip )
{
  optind = 1;
  opterr = 0;

  constexpr const char * gcc_optstring = "-l:B:o:gO::D:U:f:I:W::L:i:";

  constexpr GCCOptionData gcc_options_data[] = {
    { GCCOption::x,  "x",  required_argument, false, ' ' },

    { GCCOption::dM, "dM", no_argument, false, 'X' },

    { GCCOption::E,  "E",  no_argument, false, 'X' },
    { GCCOption::S,  "S",  no_argument, false, 'X' },
    { GCCOption::c,  "c",  no_argument, false, 'X' },

    { GCCOption::s,  "s",  no_argument, false, 'X' },
    { GCCOption::C,  "C",  no_argument, false, 'X' },

    { GCCOption::w,  "w",  no_argument, false, 'X' },
    { GCCOption::v,  "v",  no_argument, false, 'X' },

    /* -M options */
    { GCCOption::M,   "M",  no_argument, false, 'X' },
    { GCCOption::MD,  "MD", no_argument, false, 'X' },
    { GCCOption::MP,  "MP", no_argument, false, 'X' },
    { GCCOption::MT,  "MT", required_argument, false, ' ' },
    { GCCOption::MF,  "MF", required_argument, false, ' ' },
    { GCCOption::MMD, "MMD", no_argument, false, ' ' },

    { GCCOption::P,          "P",        no_argument, false, 'X' },
    { GCCOption::pg,         "pg",       no_argument, false, 'X' },
    { GCCOption::pie,        "pie",      no_argument, false, 'X' },
    { GCCOption::pthread,    "pthread",  no_argument, false, 'X' },
    { GCCOption::shared,     "shared",   no_argument, false, 'X' },
    { GCCOption::pipe,       "pipe",     no_argument, false, 'X' },
    { GCCOption::pedantic,   "pedantic", no_argument, false, 'X' },
    { GCCOption::nostdlib,   "nostdlib", no_argument, false, 'X' },
    { GCCOption::nostdinc,   "nostdinc", no_argument, false, 'X' },
    { GCCOption::dashstatic, "static",   no_argument, false, 'X' },
    { GCCOption::mfentry,    "mfentry",  no_argument, false, 'X' },
    { GCCOption::gdwarf_4,   "gdwarf-4", no_argument, false, 'X' },
    { GCCOption::rdynamic,   "rdynamic", no_argument, false, 'X' },

    { GCCOption::mno_sse,           "mno-sse",           no_argument, false, 'X' },
    { GCCOption::mno_mmx,           "mno-mmx",           no_argument, false, 'X' },
    { GCCOption::mno_sse2,          "mno-sse2",          no_argument, false, 'X' },
    { GCCOption::mno_3dnow,         "mno-3dnow",         no_argument, false, 'X' },
    { GCCOption::mno_avx,           "mno-avx",           no_argument, false, 'X' },
    { GCCOption::m32,               "m32",               no_argument, false, 'X' },
    { GCCOption::m64,               "m64",               no_argument, false, 'X' },
    { GCCOption::mno_80387,         "mno-80387",         no_argument, false, 'X' },
    { GCCOption::mno_fp_ret_in_387, "mno-fp-ret-in-387", no_argument, false, 'X' },
    { GCCOption::mno_red_zone,      "mno-red-zone",      no_argument, false, 'X' },
    { GCCOption::msoft_float,       "msoft-float",       no_argument, false, 'X' },
    { GCCOption::dumpversion,       "dumpversion",       no_argument, false, 'X' },
    { GCCOption::dumpmachine,       "dumpmachine",       no_argument, false, 'X' },

    { GCCOption::mskip_rax_setup,   "mskip-rax-setup",   no_argument, false, 'X' },
    { GCCOption::msse4_2,           "msse4.2",           no_argument, false, 'X' },

    { GCCOption::include, "include", required_argument, false, ' ' },
    { GCCOption::param,   "param",   required_argument, true,  ' ' },
    { GCCOption::std,     "std",     required_argument, false, '=' },
    { GCCOption::mcmodel, "mcmodel", required_argument, false, '=' },
    { GCCOption::Xlinker, "Xlinker", required_argument, false, ' ' },
    { GCCOption::mtune,   "mtune",   required_argument, false, '=' },
    { GCCOption::march,   "march",   required_argument, false, '=' },
    { GCCOption::isystem, "isystem", required_argument, false, ' ' },

    { GCCOption::mpreferred_stack_boundary, "mpreferred-stack-boundary", required_argument, false, '=' },

    { GCCOption::mno_tls_direct_seg_refs,   "mno-tls-direct-seg-refs",   no_argument, false, 'X' },

    { GCCOption::mregparm, "mregparm", required_argument, false, '=' },

    { GCCOption::print_file_name, "print-file-name", required_argument, false, '=' },
  };

  constexpr size_t option_count = sizeof( gcc_options_data ) / sizeof( GCCOptionData);
  constexpr auto gcc_options = getopt_options<option_count>{ gcc_options_data };

  map<GCCOption, GCCOptionData> options_map;
  for ( size_t i = 0; i < option_count; i++ ) {
    options_map.insert( make_pair( gcc_options_data[ i ].option, gcc_options_data[ i ] ) );
  }

  Language current_language = Language::NONE; /* -x arugment */

  while ( true ) {
    const int opt = getopt_long_only( argc, argv, gcc_optstring, gcc_options.list, nullptr );
    const GCCOption gccopt = static_cast<GCCOption>( opt );

    if ( opt == -1 ) { break; }

    /* detect non-option argument */
    if ( opt == 1 ) {
      string input_file { optarg };
      Language file_lang = current_language;

      if ( file_lang == Language::NONE ) {
        if ( input_file == "-" and last_stage_.initialized() and *last_stage_ == PREPROCESS ) {
          file_lang = Language::C;
        }
        else {
          file_lang = GCCModelGenerator::filename_to_language( input_file );
        }
      }

      add_input( input_file, file_lang );

      continue;
    }

    bool flag_processed = true;

    switch ( opt ) {
    case 'l':
      add_input( optarg, Language::SHARED_LIBRARY );
      break;

    case 'i':
      if ( startswith( optarg, "system" ) ) {
        string optarg_str { optarg };
        include_dirs_.push_back( optarg_str.substr( strlen( "system" ) ) );
      }
      else {
        flag_processed = false;
      }
      break;

    case 'o':
      output_ = optarg;
      break;

    case 'I':
      args_.push_back( string( "-I" ) + optarg );
      include_dirs_.emplace_back( optarg );
      break;

    case 'L':
      args_.push_back( string( "-L" ) + optarg );
      library_dirs_.emplace_back( optarg );
      break;

    case 'g':
      if ( not force_strip_ ) {
        add_option( GCCOption::g, "g" );
      }
      break;

    case 'O': add_option( GCCOption::O, "O", optarg, '\0' ); break;
    case 'D': add_option( GCCOption::D, "D", optarg, '\0' ); break;
    case 'U': add_option( GCCOption::U, "U", optarg, '\0' ); break;
    case 'f': add_option( GCCOption::f, "f", optarg, '\0' ); break;

    case 'W':
      process_W_option( optarg ? optarg : "" );
      break;

    case 'B':
      throw runtime_error( "illegal -B flag" );

    default:
      flag_processed = false;
    }

    bool add_to_args = true;

    if ( not flag_processed ) {
      switch ( gccopt ) {
      case GCCOption::MMD:
        add_option( options_map.at( GCCOption::MD ), optarg );
        add_to_args = false;
        break;

      case GCCOption::E:
        last_stage_ = PREPROCESS;
        break;

      case GCCOption::S:
        last_stage_ = ( not last_stage_.initialized() or *last_stage_ >= COMPILE ) ? COMPILE : *last_stage_;
        break;

      case GCCOption::c:
        last_stage_ = ( not last_stage_.initialized() or *last_stage_ >= ASSEMBLE ) ? ASSEMBLE : *last_stage_;
        break;

      case GCCOption::x:
        input_args_.emplace_back( "-x" );
        input_args_.emplace_back( optarg );
        current_language = GCCModelGenerator::name_to_language( optarg );
        add_to_args = false;
        break;

      case GCCOption::isystem:
        include_dirs_.emplace_back( optarg );
        break;

      case GCCOption::nostdlib:
        no_stdlib_ = true;
        break;

      case GCCOption::nostdinc:
        no_stdinc_ = true;
        break;

      default:
        if ( not options_map.count( gccopt ) ) {
          throw runtime_error( "unknown gcc flag: " + string( argv[ optind - 1 ] ) );
        }
      }

      if ( add_to_args ) {
        add_option( options_map.at( gccopt ), optarg );
      }
    }
  }

  for ( InputFile & input : input_files_ ) {
    input.index += args_.size() + ( output_.empty() ? 0 : 2 );
  }
}

void GCCArguments::process_W_option( const string & optarg )
{
  bool accepted = true;

  if ( optarg.size() == 0 ) {
    args_.push_back( "-W" );
    return;
  }

  static const string VERSION_SCRIPT = "--version-script=";
  static const string LINKER_SCRIPT = "-T,";

  if ( optarg.length() >= 2 and optarg[ 1 ] == ',' ) {
    accepted = false;

    const string suboptarg = optarg.substr( 2 );

    switch ( optarg[ 0 ] ) {
    case 'p':
      if ( startswith( suboptarg, "-MD" ) ) {
        string::size_type comma = suboptarg.find( ',' );

        if ( comma != string::npos ) {
          add_option( GCCOption::MD, "MD" );
          add_option( GCCOption::MF, "MF", suboptarg.substr( comma + 1 ).c_str(), ' ' );
          return; /* don't need to add the original argument */
        }
      }
      else if ( startswith( suboptarg, "-D" ) ) {
        accepted = true;
      }

      break;

    case 'l':
      /* XXX we're accepting all of -Wl,XYZ options... */
      accepted = true;

      if ( startswith( suboptarg, VERSION_SCRIPT ) ) {
        extra_infiles_[ LINK ].emplace_back( suboptarg.substr( VERSION_SCRIPT.size() ) );
      }
      else if ( startswith( suboptarg, LINKER_SCRIPT ) ) {
        extra_infiles_[ LINK ].emplace_back( suboptarg.substr( LINKER_SCRIPT.size() ) );
      }

      break;

    case 'a':
      /* XXX allow all */
      accepted = true;
      if ( suboptarg.find( ',' ) == string::npos ) {
        if ( roost::exists( suboptarg ) ) {
          extra_infiles_[ ASSEMBLE ].emplace_back( suboptarg );
        }
      }

      break;
    }
  }

  if ( accepted ) {
    args_.push_back( "-W" + optarg );
  }
  else {
    throw runtime_error( "not implemented: -W" + optarg );
  }
}

void GCCArguments::add_option( const GCCOptionData & option_data,
                               const char * optarg )
{
  add_option( option_data.option, option_data.option_str, optarg ? optarg : "",
              ( option_data.has_arg == no_argument ) ? 'X' : option_data.arg_separator,
              option_data.double_dash );
}

void GCCArguments::add_option( const GCCOption option, const string & optstr,
                               const char * value, const char arg_separator,
                               const bool double_dash )
{
  const size_t index = args_.size();

  const string actual_optstr = ( double_dash ? "--" : "-" ) + optstr;
  const string actual_value = value ? value : string();

  if ( arg_separator == 'X' ) {
    /* this option doesn't have an argument */
    args_.emplace_back( actual_optstr );
  }
  else if ( arg_separator == ' ' ) {
    args_.emplace_back( actual_optstr );
    args_.emplace_back( actual_value );
  }
  else if ( arg_separator == '\0') {
    args_.emplace_back( actual_optstr + actual_value );
  }
  else {
    args_.emplace_back( actual_optstr + arg_separator + actual_value );
  }

  opt_map_.insert( { option, { index, actual_value } } );
}

void GCCArguments::add_input( const string & filename, const Language language )
{
  if ( language == Language::SHARED_LIBRARY ) {
    input_args_.emplace_back( string( "-l" ) + filename );

    input_files_.push_back( { filename, language, language, input_args_.size() - 1,
                              ThunkFactory::Data() } );
  }
  else {
    input_args_.push_back( filename );

    input_files_.push_back( { filename, language, language, input_args_.size() - 1,
                              ThunkFactory::Data( filename ) } );
  }
}

vector<string> GCCArguments::all_args() const
{
  vector<string> result;
  result.reserve( args_.size() + input_args_.size() + output_.empty() ? 0 : 2 );

  result.insert( result.end(), args_.begin(), args_.end() );

  if ( not output_.empty() ) {
    result.emplace_back( "-o" );
    result.push_back( output_ );
  }

  result.insert( result.end(), input_args_.begin(), input_args_.end() );

  return result;
}

Optional<string> GCCArguments::option_argument( const GCCOption option ) const
{
  if ( opt_map_.count( option ) ) {
    return make_optional<string>( true, opt_map_.at( option ).second );
  }

  return {};
}

const vector<string> & GCCArguments::extra_infiles( const GCCStage stage )
{
  if ( extra_infiles_.count( stage ) == 0 ) {
    extra_infiles_.emplace( piecewise_construct, forward_as_tuple( stage ), forward_as_tuple( 0 ) );
  }

  return extra_infiles_.at( stage );
}

void GCCArguments::print_args() const
{
  for ( const string & arg : all_args() ) {
    cerr << arg << " ";
  }

  cerr << endl;
}
