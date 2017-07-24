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
#include <boost/tokenizer.hpp>
#include <getopt.h>
#include <libgen.h>

#include "exception.hh"
#include "optional.hh"
#include "digest.hh"
#include "temp_file.hh"
#include "system_runner.hh"
#include "thunk.hh"
#include "utils.hh"

#include "model-gcc.hh"

using namespace std;
using namespace boost;
using namespace gg::thunk;

/* TODO read this information from a config file */
static const string GCC = "gcc";
static const string AS  = "as";
static const string CC1 = "cc1";
static const string COLLECT2 = "collect2";
static const string LD = "ld";
static const string GG_BIN_PREFIX = "/usr/bin/";
static const roost::path toolchain_path { std::string( TOOLCHAIN_PATH ) };

static auto gcc_function =
  []( const vector<string> & args ) -> Function
  {
    return { GG_BIN_PREFIX + GCC, args, {}, program_hash( GCC ) };
  };

static const unordered_map<string, InFile> program_infiles {
  {
    GCC,
    { GG_BIN_PREFIX + GCC, ( toolchain_path / GCC ).string(), program_hash( GCC ), 0 }
  },
  {
    CC1,
    { GG_BIN_PREFIX + CC1, ( toolchain_path / CC1 ).string(), program_hash( CC1 ), 0 }
  },
  {
    AS,
    { GG_BIN_PREFIX + AS, ( toolchain_path / AS ).string(), program_hash( AS ), 0 }
  },
  {
    COLLECT2,
    { GG_BIN_PREFIX + COLLECT2, ( toolchain_path / COLLECT2 ).string(), program_hash( COLLECT2 ), 0 }
  },
  {
    LD,
    { GG_BIN_PREFIX + LD, ( toolchain_path / LD ).string(), program_hash( LD ), 0 }
  },
};

void dump_gcc_specs( TempFile & target_file )
{
  array<char, 4096> buffer;

  std::shared_ptr<FILE> readpipe( popen( "gcc -dumpspecs", "r" ), pclose );

  if ( !readpipe ) {
    throw runtime_error( "could not execute `gcc -dumpspecs`." );
  }

  while ( !feof( readpipe.get() ) ) {
    if ( fgets( buffer.data(), 4096, readpipe.get() ) != nullptr ) {
      string result = buffer.data();
      target_file.write( result );
    }
  }
}

vector<string> get_preprocess_dependencies( const vector<string> & gcc_args,
                                            const string & specsfile )
{
  vector<string> args;
  args.reserve( 2 + gcc_args.size() );
  args.push_back( "gcc" );
  args.push_back( "-specs=" + specsfile );
  args.insert( args.end(), gcc_args.begin(), gcc_args.end() );

  string dep_out_filename;

  auto has_dependencies_option = find_if(
    args.begin(), args.end(),
    []( const string & opt )
    {
      return ( opt == "-M" ) or ( opt == "-MF" ) or ( opt == "-MM" ) or
             ( opt == "-MG" ) or ( opt == "-MP" ) or ( opt == "-MQ" ) or
             ( opt == "-MD" ) or ( opt == "-MMD" );
    }
  );

  if ( has_dependencies_option != args.end() ) {
    throw runtime_error( "find dependencies: command already has -M flag" );
  }

  {
    UniqueFile gcc_mf_output { "/tmp/gg-model-gcc-mf" };
    dep_out_filename = gcc_mf_output.name();

    /* XXX we should probably get rid of -o option */
    args.push_back( "-M" );
    args.push_back( "-MF" );
    args.push_back( dep_out_filename );
  }

  run( args[ 0 ], args, {}, true, true );

  vector<string> dependencies;

  ifstream depin { dep_out_filename };
  string line;
  bool first_line = true;

  while ( getline( depin, line ) ) {
    if ( first_line ) {
      line = line.substr( line.find(':') + 2, line.length() );
      first_line = false;
    }
    else {
      line = line.substr( 1, line.length() );
    }

    if ( line[ line.length() - 1 ] == '\\' ) {
      line = line.substr( 0, line.length() - 2 );
    }

    if ( line == "\\" ) {
      continue;
    }

    tokenizer<escaped_list_separator<char>> tok( line, { "\\", " ", "\"\'" } );

    for ( auto t = tok.begin(); t != tok.end(); t++ ) {
      dependencies.push_back( *t );
    }
  }

  return dependencies;
}

Language filename_to_language( const string & path )
{
  vector<char> path_cstr( path.c_str(), path.c_str() + path.size() + 1 );
  const string filename { basename( path_cstr.data() ) };
  const auto pos = filename.find_last_of( '.' );

  if ( pos == string::npos ) {
    /* did not find a file extension! */
  }

  string extension = filename.substr( pos + 1 );

  if ( extension == "c" ) return Language::C;
  if ( extension == "h" ) return Language::C_HEADER;
  if ( extension == "i" ) return Language::CPP_OUTPUT;
  if ( extension == "s" ) return Language::ASSEMBLER;
  if ( extension == "o" ) return Language::OBJECT;
  if ( extension == "a" ) return Language::ARCHIVE_LIBRARY;

  throw runtime_error( "unknown file extension" );
}

Language name_to_language( const string & name )
{
  if ( name == "none" ) return Language::NONE;
  if ( name == "c" ) return Language::C;
  if ( name == "c-header" ) return Language::C_HEADER;
  if ( name == "cpp-output" ) return Language::CPP_OUTPUT;
  if ( name == "assembler" ) return Language::ASSEMBLER;

  throw runtime_error( "unknown language name" );
}

GCCStage language_to_stage( const Language lang )
{
  switch ( lang ) {
  case Language::C: return PREPROCESS;
  case Language::C_HEADER: return PREPROCESS;
  case Language::CPP_OUTPUT: return COMPILE;
  case Language::ASSEMBLER: return ASSEMBLE;
  case Language::OBJECT: return LINK;

  default: throw runtime_error( "unknown language" );
  }
}

string stage_output_name( const GCCStage stage, const string basename )
{
  string result = basename;

  switch ( stage ) {
  case PREPROCESS: return result + ".i";
  case COMPILE: return result + ".s";
  case ASSEMBLE: return result + ".o";
  case LINK: return result + ".out";

  default:
    throw runtime_error( "invalid GCCstage stage" );
  }
}

bool is_non_object_input( const InputFile & input )
{
  switch( input.language ) {
  case Language::SHARED_LIBRARY:
  case Language::ARCHIVE_LIBRARY:
  case Language::OBJECT:
    return false;

  default: return true;
  }
}

Thunk generate_thunk( const GCCStage stage, const vector<string> original_args,
                      const string & input, const string & output,
                      const string & specsfile )
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

    return { output, gcc_function( all_args ), base_infiles };
  }

  case COMPILE:
    args.push_back( "-S" );
    base_infiles.push_back( program_infiles.at( CC1 ) );

    return { output, gcc_function( args ), base_infiles };

  case ASSEMBLE:
    args.push_back( "-c" );
    base_infiles.push_back( program_infiles.at( AS ) );
    return { output, gcc_function( args ), base_infiles };

  default: throw runtime_error( "not implemented" );
  }
}

Thunk generate_link_thunk( const vector<InputFile> & link_inputs,
                           const vector<string> & link_args,
                           const vector<string> & dependencies,
                           const string & output )
{
  vector<string> args { link_args.begin(), link_args.end() };

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

  for ( const string & dir : c_library_path ) {
    infiles.emplace_back( dir, InFile::Type::DUMMY_DIRECTORY );
  }

  infiles.emplace_back( "/__gg__/bin/.", InFile::Type::DUMMY_DIRECTORY );
  infiles.emplace_back( "/usr/lib/gcc/.", InFile::Type::DUMMY_DIRECTORY );

  vector<string> all_args;
  all_args.reserve( c_library_path.size() + args.size() );
  //all_args.push_back( "-specs=/__gg__/gcc-specs" );
  for ( const auto & p : c_library_path ) {
    all_args.push_back( "-Wl,-rpath-link," + p );
  }

  for ( const auto & p : c_library_path ) {
    all_args.push_back( "-L" + p );
  }

  all_args.insert( all_args.end(), args.begin(), args.end() );

  return { output, gcc_function( all_args ), infiles };
}

int main( int argc, char * argv[] )
{
  roost::path gg_dir = gg::models::create_gg_dir();

  Language current_langauge = Language::NONE; /* -x arugment */
  Optional<GCCStage> last_stage;

  vector<string> args;

  for ( int i = 1; i < argc; i++ ) {
    args.push_back( argv[ i ] );
  }

  string last_stage_output_filename {};

  vector<InputFile> input_files;

  const option gcc_options[] = {
    { NULL, required_argument, NULL, 'x' }, /* Explicitly specify the language for the following input files */

    { NULL, no_argument,       NULL, 'E' }, /* Stop after the preprocessing stage */
    { NULL, no_argument,       NULL, 'S' }, /* Stop after the stage of compilation proper */
    { NULL, no_argument,       NULL, 'c' }, /* Compile or assemble the source files, but do not link */

    { NULL, required_argument, NULL, 'o' }, /*  Output file name */

    { 0, 0, 0, 0 },
  };

  optind = 1; /* reset getopt */
  opterr = 0; /* turn off error messages */

  while ( true ) {
    const int opt = getopt_long( argc, argv, "-x:ESco:l:W:O:B:", gcc_options, nullptr );
    size_t arg_index = static_cast<size_t>( optind - 2 );

    /* detect the end of options */
    if ( opt == -1 ) {
      break;
    }

    /* detect non-option argument */
    if ( opt == 1 ) {
      string input_file { optarg };
      Language file_lang = current_langauge;

      if ( current_langauge == Language::NONE ) {
        file_lang = filename_to_language( input_file );
      }

      input_files.push_back( { input_file, file_lang, file_lang, arg_index } );

      continue;
    }

    switch ( opt ) {
    case 'l':
      input_files.push_back( { optarg, Language::SHARED_LIBRARY,
                               Language::SHARED_LIBRARY, arg_index } );
      break;

    case 'x':
      current_langauge = name_to_language( optarg );
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
      break;

    case 'B':
      throw runtime_error( "illegal -B flag" );
    }
  }

  if ( not last_stage.initialized() ) {
    last_stage = LINK;
  }

  if ( input_files.size() == 0 ) {
    throw runtime_error( "no input files" );
  }

  /* push special gg prefix for gcc binaries */
  args.push_back( "-B" + GG_BIN_PREFIX );

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
      Thunk stage_thunk = generate_thunk( stage, args_stage,
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

    for ( auto const & link_input : link_inputs ) {
      if ( not ( link_input.source_language == Language::OBJECT or
                 link_input.source_language == Language::ARCHIVE_LIBRARY or
                 link_input.source_language == Language::SHARED_LIBRARY ) ) {
        link_args[ link_input.index ] = link_input.name;
      }
    }

    vector<string> dependencies = get_link_dependencies( link_inputs, args );

    link_args.push_back( "-B/usr/lib/gcc" );

    Thunk thunk = generate_link_thunk( link_inputs, link_args, dependencies,
                                       last_stage_output_filename );
    thunk.store( gg_dir );
  }

  return 0;
}
