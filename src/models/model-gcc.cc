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
#include <boost/filesystem.hpp>
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

#include "toolchain.hh"

using namespace std;
using namespace boost;
using namespace gg::thunk;

namespace fs = boost::filesystem;

/* TODO read this information from a config file */
static const string GCC = "gcc";
static const string AS  = "as";
static const string CC1 = "cc1";
static const string GCC_BIN_PREFIX = "/__gg__/bin/";
static const fs::path toolchain_path { TOOLCHAIN_PATH };

static auto gcc_function =
  []( const vector<string> & args ) -> Function
  {
    return { GCC_BIN_PREFIX + GCC, args, program_hash( GCC ) };
  };

static const unordered_map<string, InFile> program_infiles {
  {
    GCC,
    { GCC_BIN_PREFIX + GCC, ( toolchain_path / GCC ).string(), program_hash( GCC ), 0 }
  },
  {
    CC1,
    { GCC_BIN_PREFIX + CC1, ( toolchain_path / CC1 ).string(), program_hash( CC1 ), 0 }
  },
  {
    AS,
    { GCC_BIN_PREFIX + AS, ( toolchain_path / AS ).string(), program_hash( AS ), 0 }
  },
};

/* TODO this should be populated based on system gcc */
static const vector<string> library_path = {
  "/usr/local/lib/x86_64-linux-gnu",
  "/lib/x86_64-linux-gnu",
  "/usr/lib/x86_64-linux-gnu",
  "/usr/local/lib64",
  "/lib64",
  "/usr/lib64",
  "/usr/local/lib",
  "/lib",
  "/usr/lib",
  "/usr/x86_64-linux-gnu/lib64",
  "/usr/x86_64-linux-gnu/lib",
};

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
  ASSEMBLER,
  OBJECT
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

vector<string> get_dependencies( const vector<string> & gcc_args )
{
  vector<string> args { gcc_args };
  args.insert( args.begin(), "gcc" );

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

  run( args, {}, true, true );

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

Thunk generate_thunk( const GCCStage stage, const vector<string> original_args,
                      const string & input, const string & output )
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

  if ( not found_output_flag ) {
    args.push_back( "-o" );
    args.push_back( output );
  }

  switch ( stage ) {
  case PREPROCESS:
  {
    vector<string> dependencies = get_dependencies( args );
    vector<InFile> preprocess_infiles {
      input,
      program_infiles.at( GCC ), program_infiles.at( CC1 )
    };

    for ( const string & dep : dependencies ) {
      preprocess_infiles.emplace_back( dep );
    }

    for ( const string & dir : c_include_path ) {
      preprocess_infiles.emplace_back( dir, InFile::Type::DUMMY_DIRECTORY );
    }

    preprocess_infiles.emplace_back( ".", InFile::Type::DUMMY_DIRECTORY );

    args.push_back( "-E" );

    vector<string> all_args;
    all_args.reserve( c_include_path.size() + args.size() + 1 );
    all_args.push_back( "-nostdinc" );
    for ( const auto & p : c_include_path ) {
      all_args.push_back( "-isystem" + p );
    }

    all_args.insert( all_args.end(), args.begin(), args.end() );

    return {
      output,
      gcc_function( all_args ),
      preprocess_infiles
    };
  }

  case COMPILE:
    args.push_back( "-S" );
    return {
      output,
      gcc_function( args ),
      {
        input,
        program_infiles.at( GCC ), program_infiles.at( CC1 )
      }
    };

  case ASSEMBLE:
    args.push_back( "-c" );
    return { output,
      gcc_function( args ),
      {
        input,
        program_infiles.at( GCC ), program_infiles.at( AS )
      }
    };

  default: throw runtime_error( "not implemented" );
  }
}

int main( int argc, char * argv[] )
{
  fs::path gg_dir = gg::models::gg_dir();

  Language current_langauge = Language::NONE; /* -x arugment */
  Optional<GCCStage> last_stage;

  vector<string> args;

  for ( int i = 1; i < argc; i++ ) {
    args.push_back( argv[ i ] );
  }

  string last_stage_output_filename {};

  vector<pair<string, Language>> input_files;

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
    const int opt = getopt_long( argc, argv, "-x:ESco:", gcc_options, nullptr );

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

      input_files.emplace_back( input_file, file_lang );

      continue;
    }

    switch ( opt ) {
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

  if ( input_files.size() == 0 ) {
    throw runtime_error( "no input files" );
  }

  if ( input_files.size() > 1 ) {
    throw runtime_error( "accepting multiple input files is not supported yet" );
  }

  if ( not last_stage.initialized() ) {
    last_stage = LINK;
  }

  const auto & input = input_files.back();

  const size_t input_idx = distance( args.begin(), find( args.begin(), args.end() , input.first ) );
  assert( input_idx < args.size() );

  args.push_back( "-B" + GCC_BIN_PREFIX );

  GCCStage first_stage = language_to_stage( input.second );

  if ( last_stage_output_filename.length() == 0 ) {
    switch ( *last_stage ) {
    case LINK:
      last_stage_output_filename = "a.out";
      break;

    default:
      last_stage_output_filename = stage_output_name( *last_stage, input.first );
    }
  }

  /* create the gg directory, if it doesn't exist */
  if ( not fs::create_directories( gg_dir ) ) {
    if ( not fs::is_directory( gg_dir ) ) {
      throw runtime_error( "cannot create gg dir" );
    }
  }

  /* input hash */
  string input_hash = digest::SHA256( input.first ).hexdigest();

  /* stage -> output_name */
  map<size_t, string> stage_output;
  stage_output[ first_stage - 1 ] = input.first;

  for ( size_t stage_num = first_stage; stage_num <= *last_stage; stage_num++ ) {
    GCCStage stage = static_cast<GCCStage>( stage_num );

    string output_name = ( stage == *last_stage ) ? last_stage_output_filename
                                                  : stage_output_name( stage, input_hash );


    vector<string> args_stage = args;
    args_stage[ input_idx ] = stage_output[ stage - 1 ];
    Thunk stage_thunk = generate_thunk( stage, args_stage,
                                        stage_output[ stage - 1 ], output_name );

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
      /* generate assemble thunk */
      cerr << ">> assembling " << stage_output[ stage - 1 ] << endl;
      break;

    case LINK:
      /* generate link thunk */
      cerr << ">> linking " << stage_output[ stage - 1 ] << endl;
      break;
    }

    stage_output[ stage ] = output_name;
    cerr << "[output=" << output_name << "]" << endl;
  }

  return 0;
}
