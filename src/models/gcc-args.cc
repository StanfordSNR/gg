/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "model-gcc.hh"
#include "gcc-args.hh"

#include <iostream>

using namespace std;

GCCArguments::GCCArguments( const int argc, char ** argv )
{
  optind = 1;
  opterr = 0;

  constexpr const char * gcc_optstring = "-l:B:ESco:gO::D:f:I:W:";
  constexpr option gcc_options[] = {
    { "x", required_argument, NULL, to_underlying( GCCOption::x ) },
    { "M", no_argument, NULL, to_underlying( GCCOption::M ) },
    { "MD", no_argument, NULL, to_underlying( GCCOption::MD ) },
    { "MP", no_argument, NULL, to_underlying( GCCOption::MP ) },
    { "MT", required_argument, NULL, to_underlying( GCCOption::MT ) },
    { "MF", required_argument, NULL, to_underlying( GCCOption::MF ) },

    { "pie", no_argument, NULL, to_underlying( GCCOption::pie ) },
    { "pthread", no_argument, NULL, to_underlying( GCCOption::pthread ) },
    { "shared", no_argument, NULL, to_underlying( GCCOption::shared ) },

    { "include", required_argument, NULL, to_underlying( GCCOption::include ) },

    { 0, 0, 0, 0 },
  };

  Language current_language = Language::NONE; /* -x arugment */

  while ( true ) {
    const int opt = getopt_long_only( argc, argv, gcc_optstring, gcc_options, nullptr );
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

    case 'I':
      args_.push_back( string ( "-I" ) + ( optarg ? optarg : "" ) );
      include_dirs_.emplace_back( optarg );
      break;

    case 'g': args_.push_back( "-g" ); break;
    case 'O': args_.push_back( string ( "-O" ) + ( optarg ? optarg : "" ) ); break;
    case 'D': args_.push_back( string ( "-D" ) + ( optarg ? optarg : "" ) ); break;
    case 'f': args_.push_back( string ( "-f" ) + ( optarg ? optarg : "" ) ); break;
    case 'W': args_.push_back( string ( "-W" ) + ( optarg ? optarg : "" ) ); break;


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
      case GCCOption::shared: add_option( gccopt, "-pthread" ); break;

      case GCCOption::include: add_option( gccopt, "-include", optarg ); break;

      default:
        throw runtime_error( "unknown gcc flag: " + string( argv[ optind - 1 ] ) );
      }
    }
  }

  for ( InputFile & input : input_files_ ) {
    input.index += args_.size() + ( output_.empty() ? 0 : 2 );
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

  input_files_.push_back( { filename, language, language, input_args_.size() - 1 } );
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
