/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "model-gcc.hh"

#include <iostream>
#include <getopt.h>

using namespace std;

struct GCCOptionData
{
  GCCOption option;
  const char * option_str;
  int has_arg;
  bool double_dash;
  char arg_separator;
};

template <typename E>
constexpr auto to_underlying( E e ) noexcept
{
    return static_cast<std::underlying_type_t<E>>( e );
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

GCCArguments::GCCArguments( const int argc, char ** argv )
{
  optind = 1;
  opterr = 0;

  constexpr const char * gcc_optstring = "-l:B:EScso:gO::D:f:I:W:L:";

  constexpr GCCOptionData gcc_options_data[] = {
    { GCCOption::x,  "x",  required_argument, false, ' ' },

    /* -M options */
    { GCCOption::M,  "M",  no_argument, false, '\0' },
    { GCCOption::MD, "MD", no_argument, false, '\0' },
    { GCCOption::MP, "MP", no_argument, false, '\0' },
    { GCCOption::MT, "MT", required_argument, false, ' ' },
    { GCCOption::MF, "MF", required_argument, false, ' ' },

    { GCCOption::pie,        "pie",      no_argument, false, '\0' },
    { GCCOption::pthread,    "pthread",  no_argument, false, '\0' },
    { GCCOption::shared,     "shared",   no_argument, false, '\0' },
    { GCCOption::pipe,       "pipe",     no_argument, false, '\0' },
    { GCCOption::pedantic,   "pedantic", no_argument, false, '\0' },
    { GCCOption::nostdlib,   "nostdlib", no_argument, false, '\0' },
    { GCCOption::nostdinc,   "nostdinc", no_argument, false, '\0' },
    { GCCOption::dashstatic, "static",   no_argument, false, '\0' },
    { GCCOption::mfentry,    "mfentry",  no_argument, false, '\0' },
    { GCCOption::gdwarf_4,   "gdwarf-4", no_argument, false, '\0' },

    { GCCOption::include, "include", required_argument, false, ' ' },
    { GCCOption::param,   "param",   required_argument, true, ' ' },
    { GCCOption::std,     "std",     required_argument, false, '=' },
    { GCCOption::mcmodel, "mcmodel", required_argument, false, '=' },
    { GCCOption::Xlinker, "Xlinker", required_argument, false, ' ' },
  };

  constexpr size_t option_count = sizeof( gcc_options_data ) / sizeof( GCCOptionData);
  constexpr auto gcc_options = getopt_options<option_count>{ gcc_options_data };

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
        file_lang = GCCModelGenerator::filename_to_language( input_file );
      }

      add_input( input_file, file_lang );

      continue;
    }

    bool flag_processed = true;

    switch ( opt ) {
    case 'l':
      add_input( optarg, Language::SHARED_LIBRARY );
      break;

    case 'E':
      add_option( GCCOption::E, "-E" );
      last_stage_ = PREPROCESS;
      break;

    case 'S':
      add_option( GCCOption::S, "-S" );
      last_stage_ = ( not last_stage_.initialized() or *last_stage_ >= COMPILE ) ? COMPILE : *last_stage_;
      break;

    case 'c':
      add_option( GCCOption::c, "-c" );
      last_stage_ = ( not last_stage_.initialized() or *last_stage_ >= ASSEMBLE ) ? ASSEMBLE : *last_stage_;
      break;

    case 'o':
      output_ = optarg;
      break;

    case 's':
      add_option( GCCOption::s, "-s" );
      break;

    case 'I':
      args_.push_back( string( "-I" ) + optarg );
      include_dirs_.emplace_back( optarg );
      break;

    case 'L':
      args_.push_back( string( "-L" ) + optarg );
      library_dirs_.emplace_back( optarg );
      break;

    case 'g': args_.push_back( "-g" ); break;
    case 'O': args_.push_back( string ( "-O" ) + ( optarg ? optarg : "" ) ); break;
    case 'D': args_.push_back( string ( "-D" ) + ( optarg ? optarg : "" ) ); break;
    case 'f': args_.push_back( string ( "-f" ) + ( optarg ? optarg : "" ) ); break;

    case 'W':
      process_W_option( optarg );
      break;


    case 'B':
      throw runtime_error( "illegal -B flag" );

    default:
      flag_processed = false;
    }

    if ( not flag_processed ) {
      switch ( gccopt ) {
      case GCCOption::x:
        input_args_.emplace_back( "-x" );
        input_args_.emplace_back( optarg );

        current_language = GCCModelGenerator::name_to_language( optarg );
        break;

      case GCCOption::M:  add_option( gccopt, "-M" ); break;
      case GCCOption::MD: add_option( gccopt, "-MD" ); break;
      case GCCOption::MP: add_option( gccopt, "-MP" ); break;
      case GCCOption::MT: add_option( gccopt, "-MT", optarg ); break;
      case GCCOption::MF: add_option( gccopt, "-MF", optarg ); break;

      case GCCOption::pie: add_option( gccopt, "-pie" ); break;
      case GCCOption::pthread: add_option( gccopt, "-pthread" ); break;
      case GCCOption::shared: add_option( gccopt, "-shared" ); break;
      case GCCOption::pipe: add_option( gccopt, "-pipe" ); break;
      case GCCOption::pedantic: add_option( gccopt, "-pedantic" ); break;
      case GCCOption::dashstatic: add_option( gccopt, "-static" ); break;

      case GCCOption::nostdlib:
        add_option( gccopt, "-nostdlib" );
        no_stdlib_ = true;
        break;

      case GCCOption::nostdinc:
        add_option( gccopt, "-nostdinc" );
        no_stdinc_ = true;
        break;

      case GCCOption::Xlinker: add_option( gccopt, "-Xlinker", optarg ); break;

      case GCCOption::include: add_option( gccopt, "-include", optarg ); break;
      case GCCOption::param: add_option( gccopt, "--param", optarg ); break;
      case GCCOption::std: add_option( gccopt, string( "--std=" ) + optarg ); break;

      case GCCOption::mcmodel: add_option( gccopt, string( "--mcmodel=" ) + optarg ); break;
      case GCCOption::mfentry: add_option( gccopt, "-mfentry" ); break;
      case GCCOption::gdwarf_4: add_option( gccopt, "-gdwarf-4" ); break;
      case GCCOption::pg: add_option( gccopt, "-pg" ); break;

      default:
        throw runtime_error( "unknown gcc flag: " + string( argv[ optind - 1 ] ) );
      }
    }
  }

  for ( InputFile & input : input_files_ ) {
    input.index += args_.size() + ( output_.empty() ? 0 : 2 );
  }
}

bool startswith( const string & str, const string & prefix ) {
  return str.compare( 0, prefix.size(), prefix ) == 0;
}

void GCCArguments::process_W_option( const string & optarg )
{
  bool accepted = true;

  static const string VERSION_SCRIPT = "--version-script=";

  if ( optarg.length() >= 2 and optarg[ 1 ] == ',' ) {
    accepted = false;

    const string suboptarg = optarg.substr( 2 );

    switch ( optarg[ 0 ] ) {
    case 'p':
      if ( startswith( suboptarg, "-MD" ) ) {
        string::size_type comma = suboptarg.find( ',' );

        if ( comma != string::npos ) {
          args_.push_back( "-MD" );
          args_.push_back( "-MF" );
          args_.push_back( suboptarg.substr( comma + 1 ) );
          return; /* don't need to add the original argument */
        }
      }

      break;

    case 'l':
      /* XXX we're accepting all of -Wl,XYZ options... */
      accepted = true;

      if ( startswith( suboptarg, VERSION_SCRIPT ) ) {
        extra_infiles_.emplace_back( suboptarg.substr( VERSION_SCRIPT.size() ) );
      }

      break;

    case 'a':
      /* XXX allow all */
      accepted = true;
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

void GCCArguments::add_option( const GCCOption option, const string & optstr,
                               const string & value )
{
  size_t index;
  args_.emplace_back( optstr );

  if ( not value.empty() ) {
    args_.emplace_back( value );
    index = args_.size() - 2;
  }
  else {
    index = args_.size() - 1;
  }

  opt_map_.insert( { option, { index, value } } );
}

void GCCArguments::add_input( const string & filename, const Language language )
{
  if ( language == Language::SHARED_LIBRARY ) {
    input_args_.emplace_back( string( "-l" ) + filename );
  }
  else {
    input_args_.push_back( filename );
  }

  input_files_.push_back( { filename, language, language, input_args_.size() - 1,
                            gg::thunk::InFile( "", "", "", 0, 0 ) } );
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

void GCCArguments::print_args() const
{
  for ( const string & arg : all_args() ) {
    cerr << arg << " ";
  }

  cerr << endl;
}
