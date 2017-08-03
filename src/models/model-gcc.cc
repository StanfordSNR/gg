/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <vector>
#include <string>
#include <tuple>
#include <algorithm>
#include <map>
#include <array>
#include <memory>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <getopt.h>
#include <libgen.h>

#include "exception.hh"
#include "optional.hh"
#include "digest.hh"
#include "temp_file.hh"
#include "thunk.hh"
#include "utils.hh"

#include "model-gcc.hh"
#include "gcc-args.hh"

using namespace std;
using namespace gg::thunk;

void dump_gcc_specs( TempFile & target_file )
{
  array<char, 4096> buffer;

  std::shared_ptr<FILE> readpipe( popen( "gcc-7 -dumpspecs", "r" ), pclose );

  if ( !readpipe ) {
    throw runtime_error( "could not execute `gcc-7 -dumpspecs`." );
  }

  while ( !feof( readpipe.get() ) ) {
    if ( fgets( buffer.data(), 4096, readpipe.get() ) != nullptr ) {
      string result = buffer.data();
      target_file.write( result );
    }
  }
}

Thunk generate_thunk( const GCCStage stage, const vector<string> original_args,
                      const vector<string> & envars, const string & input,
                      const string & output, const string & specsfile )
{
  vector<string> args { original_args };

  args.erase(
    remove_if(
      args.begin(), args.end(),
      []( const string & s ) { return ( s == "-E" or s == "-S" or s == "-c" ); }
    ), args.end()
  );

  bool found_output_flag = false;

  for ( size_t i = 0; i < args.size(); i++ ) {
    if ( args[ i ] == "-o" ) {
      if ( i + 1 == args.size() ) {
        throw runtime_error( "invalid argument: -o option with no argument" );
      }

      args[ ++i ] = output;
      found_output_flag = true;
    }
  }

  vector<InFile> base_infiles = {
    input, program_infiles.at( GCC )
  };

  if ( specsfile.length() ) {
    base_infiles.emplace_back( "/__gg__/gcc-specs", specsfile );

    if ( stage != PREPROCESS ) {
      /* For preprocess stage, we are going to use `args` to get the
         dependencies, so we can't have this FAKE specs path here. */
      args.insert( args.begin(), "-specs=/__gg__/gcc-specs" );
    }
  }

  if ( not found_output_flag ) {
    args.push_back( "-o" );
    args.push_back( output );
  }

  switch ( stage ) {
  case PREPROCESS:
  {
    vector<string> dependencies = get_preprocess_dependencies( args, specsfile );

    base_infiles.emplace_back( program_infiles.at( CC1 ) );

    for ( const string & dep : dependencies ) {
      base_infiles.emplace_back( dep );
    }

    for ( const string & dir : c_include_path ) {
      base_infiles.emplace_back( dir, InFile::Type::DUMMY_DIRECTORY );
    }

    base_infiles.emplace_back( ".", InFile::Type::DUMMY_DIRECTORY );

    args.push_back( "-E" );

    vector<string> all_args;
    all_args.reserve( c_include_path.size() + args.size() + 2 );
    all_args.push_back( "-specs=/__gg__/gcc-specs" );
    all_args.push_back( "-nostdinc" );
    for ( const auto & p : c_include_path ) {
      all_args.push_back( "-isystem" + p );
    }

    all_args.insert( all_args.end(), args.begin(), args.end() );

    return { output, gcc_function( all_args, envars ), base_infiles };
  }

  case COMPILE:
    args.push_back( "-S" );
    base_infiles.push_back( program_infiles.at( CC1 ) );

    return { output, gcc_function( args, envars ), base_infiles };

  case ASSEMBLE:
    args.push_back( "-c" );
    base_infiles.push_back( program_infiles.at( AS ) );
    return { output, gcc_function( args, envars ), base_infiles };

  default: throw runtime_error( "not implemented" );
  }
}

Thunk generate_link_thunk( const vector<InputFile> & link_inputs,
                           const vector<string> & link_args,
                           const vector<string> & envars,
                           const vector<string> & dependencies,
                           const string & output )
{
  vector<InFile> infiles;
  infiles.emplace_back( program_infiles.at( GCC ) );
  infiles.emplace_back( program_infiles.at( COLLECT2 ) );
  infiles.emplace_back( program_infiles.at( LD ) );

  for ( auto const & link_input : link_inputs ) {
    if ( link_input.source_language == Language::OBJECT or
         link_input.source_language == Language::ARCHIVE_LIBRARY ) {
      infiles.emplace_back( link_input.name );
    }
  }

  for ( const string & dep : dependencies ) {
    infiles.emplace_back( roost::path( dep ).lexically_normal().string() );
  }

  vector<string> args;
  args.reserve( link_args.size() + gcc_library_path.size() );

  for ( const string & dir : gcc_library_path ) {
    infiles.emplace_back( dir, InFile::Type::DUMMY_DIRECTORY );
    args.push_back( "-L" + dir );
  }

  for ( const string & dir : ld_search_path ) {
    infiles.emplace_back( dir, InFile::Type::DUMMY_DIRECTORY );
  }

  infiles.emplace_back( gcc_install_path, InFile::Type::DUMMY_DIRECTORY );

  args.insert( args.end(), link_args.begin(), link_args.end() );

  args.push_back( "-B" + gcc_install_path );

  return { output, gcc_function( args, envars ), infiles };
}

template <typename E>
constexpr auto to_underlying(E e) noexcept
{
    return static_cast<std::underlying_type_t<E>>(e);
}

int main( int argc, char * argv[] )
{
  roost::path gg_dir = gg::models::create_gg_dir();

  Language current_language = Language::NONE; /* -x arugment */
  Optional<GCCStage> last_stage;

  vector<InputFile> input_files;
  vector<string> args;
  vector<string> envars;
  map<GCCOption, string> arg_map;

  for ( int i = 1; i < argc; i++ ) {
    args.push_back( argv[ i ] );
  }

  /* arguments for dependency generation (gcc -M) */
  vector<string> dep_gen_args;

  string last_stage_output_filename {};

  const option gcc_options[] = {
    { "x", required_argument, NULL, to_underlying( GCCOption::x ) },
    { "M", no_argument, NULL, to_underlying( GCCOption::M ) },
    { "MD", no_argument, NULL, to_underlying( GCCOption::MD ) },
    { "MP", no_argument, NULL, to_underlying( GCCOption::MP ) },
    { "MT", required_argument, NULL, to_underlying( GCCOption::MT ) },
    { "MF", required_argument, NULL, to_underlying( GCCOption::MF ) },

    { "pie", required_argument, NULL, to_underlying( GCCOption::pie ) },

    { 0, 0, 0, 0 },
  };

  optind = 1; /* reset getopt */
  opterr = 0; /* turn off error messages */

  while ( true ) {
    const int opt = getopt_long_only( argc, argv, "-l:B:ESco:gO:D:f:", gcc_options, nullptr );
    size_t arg_index = static_cast<size_t>( optind - 2 );

    /* detect the end of options */
    if ( opt == -1 ) {
      break;
    }

    /* detect non-option argument */
    if ( opt == 1 ) {
      string input_file { optarg };
      Language file_lang = current_language;

      if ( current_language == Language::NONE ) {
        file_lang = filename_to_language( input_file );
      }

      input_files.push_back( { input_file, file_lang, file_lang, arg_index } );

      continue;
    }

    bool flag_processed = true;

    switch ( opt ) {
    case 'l':
      input_files.push_back( { optarg, Language::SHARED_LIBRARY,
                               Language::SHARED_LIBRARY, arg_index } );
      break;

    case 'E':
      last_stage = PREPROCESS;
      break;

    case 'S':
      last_stage = ( not last_stage.initialized() or *last_stage >= COMPILE ) ? COMPILE : *last_stage;
      break;

    case 'c':
      last_stage = ( not last_stage.initialized() or *last_stage >= ASSEMBLE ) ? ASSEMBLE : *last_stage;
      break;

    case 'o':
      last_stage_output_filename = optarg;
      arg_map[ GCCOption::o ] = last_stage_output_filename;
      break;

    case 'g':
    case 'O':
    case 'D':
    case 'f':
      break;

    case 'B':
      throw runtime_error( "illegal -B flag" );

    default:
      flag_processed = false;
    }

    if ( not flag_processed ) {
      GCCOption gccopt = static_cast<GCCOption>( opt );

      switch ( gccopt ) {
      case GCCOption::x:
        current_language = name_to_language( optarg );
        break;

      case GCCOption::M:
      case GCCOption::MD:
      case GCCOption::MP:
        dep_gen_args.push_back( argv[ optind - 1 ] );
        arg_map[ gccopt ] = {};
        break;

      case GCCOption::MT:
      case GCCOption::MF:
        dep_gen_args.push_back( argv[ optind - 2 ] );
        dep_gen_args.push_back( optarg );

        arg_map[ gccopt ] = optarg;

        break;

      case GCCOption::pie:
        break;

      default:
        throw runtime_error( "unknown gcc flag: " + string( argv[ optind - 1 ] ) );
      }
    }
  }

  if ( not last_stage.initialized() ) {
    last_stage = LINK;
  }

  if ( input_files.size() == 0 ) {
    throw runtime_error( "no input files" );
  }

  /* let gcc know where to find the binaries */
  envars.push_back( "PATH=" + GG_BIN_PREFIX );

  vector<InputFile> link_inputs;

  size_t non_object_inputs = std::count_if( input_files.begin(), input_files.end(), is_non_object_input );

  if ( non_object_inputs > 0 && input_files.size() > 1 ) {
    throw runtime_error( "multiple inputs are only allowed for linking" );
  }

  /* generate gcc specs file */
  TempFile specs_tmpfile { "/tmp/gccspecs" };
  dump_gcc_specs( specs_tmpfile );

  for ( const InputFile & input : input_files ) {
    if ( !is_non_object_input( input ) ) {
      link_inputs.push_back( input );
      continue;
    }

    GCCStage first_stage = language_to_stage( input.language );
    GCCStage input_last_stage = ( *last_stage == LINK ) ? ASSEMBLE : *last_stage;
    string input_hash = digest::SHA256( input.name ).hexdigest();

    map<size_t, string> stage_output;
    stage_output[ first_stage - 1 ] = input.name;

    for ( size_t stage_num = first_stage; stage_num <= input_last_stage; stage_num++ ) {
      GCCStage stage = static_cast<GCCStage>( stage_num );

      string output_name = ( stage == *last_stage ) ? last_stage_output_filename
                                                    : stage_output_name( stage, input_hash );


      vector<string> args_stage = args;
      args_stage[ input.index ] = stage_output[ stage - 1 ];
      Thunk stage_thunk = generate_thunk( stage, args_stage, envars,
                                          stage_output[ stage - 1 ], output_name,
                                          specs_tmpfile.name() );

      stage_thunk.store( gg_dir );

      switch ( stage ) {
      case PREPROCESS:
        /* generate preprocess thunk */
        cerr << ">> preprocessing " << stage_output[ stage - 1 ] << endl;
        break;

      case COMPILE:
        /* generate compile thunk */
        cerr << ">> compiling " << stage_output[ stage - 1 ] << endl;
        break;

      case ASSEMBLE:
      {
        /* generate assemble thunk */
        cerr << ">> assembling " << stage_output[ stage - 1 ] << endl;

        InputFile link_input = input;
        link_input.name = output_name;
        link_input.language = Language::OBJECT;
        link_input.source_language = input.language;
        link_input.index = input.index;
        link_inputs.push_back( link_input );

        break;
      }

      default:
        throw runtime_error( "unexcepted stage" );
      }

      stage_output[ stage ] = output_name;
      cerr << "[output=" << output_name << "]" << endl;
    }
  }

  if ( last_stage == LINK ) {
    if ( last_stage_output_filename.length() == 0 ) {
      last_stage_output_filename = "a.out";
    }

    vector<string> link_args { args };

    cerr << ">> linking ";
    for ( auto const & link_input : link_inputs ) {
      if ( not ( link_input.source_language == Language::OBJECT or
                 link_input.source_language == Language::ARCHIVE_LIBRARY or
                 link_input.source_language == Language::SHARED_LIBRARY ) ) {
        link_args[ link_input.index ] = link_input.name;
      }

      cerr << link_input.name << " ";
    }
    cerr << endl;

    vector<string> dependencies = get_link_dependencies( link_inputs, args );

    /* NOTE gg-gcc is NOT a native compiler, it is a cross compiler, so
       this doesn't work, because GCC looks for this environment variable
       only when it is configured as a NATIVE compiler. We will use -L
       flag to pass library search path. *sigh* */
    /* ostringstream libray_path_envar_ss;
    libray_path_envar_ss << "LIBRARY_PATH=";

    bool first = true;
    for ( auto const & path : gcc_library_path ) {
      if ( not first ) {
        libray_path_envar_ss << ":";
      }

      libray_path_envar_ss << path;
      first = false;
    }

    envars.push_back( libray_path_envar_ss.str() ); */

    link_args.push_back( "-Wl,-rpath-link,/lib/x86_64-linux-gnu" );

    Thunk thunk = generate_link_thunk( link_inputs, link_args, envars,
                                       dependencies, last_stage_output_filename );
    thunk.store( gg_dir );
  }

  return 0;
}
