/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <vector>
#include <string>
#include <tuple>
#include <algorithm>
#include <map>
#include <getopt.h>
#include <libgen.h>

#include "exception.hh"
#include "model-gcc.hh"

using namespace std;

enum GCCStage
{
  NOT_SET = 0,
  PREPROCESS,
  COMPILE,
  ASSEMBLE,
  LINK
};

enum Language
{
  LANGUAGE_NONE,
  LANGUAGE_C,
  LANGUAGE_C_HEADER,
  LANGUAGE_CPP_OUTPUT,
  LANGUAGE_ASSEMBLER,
  LANGUAGE_OBJECT
};

Language filename_to_language( const string & path )
{
  vector<char> path_cstr( path.c_str(), path.c_str() + path.size() + 1 );
  const string filename { basename( path_cstr.data() ) };
  const auto pos = filename.find_last_of( '.' );

  if ( pos == string::npos ) {
    /* did not find a file extension! */
  }

  string extension = filename.substr( pos + 1 );

  if ( extension == "c" ) return LANGUAGE_C;
  if ( extension == "h" ) return LANGUAGE_C_HEADER;
  if ( extension == "i" ) return LANGUAGE_CPP_OUTPUT;
  if ( extension == "s" ) return LANGUAGE_ASSEMBLER;
  if ( extension == "o" ) return LANGUAGE_OBJECT;

  throw runtime_error( "unknown file extension" );
}

Language name_to_language( const string & name )
{
  if ( name == "none" ) return LANGUAGE_NONE;
  if ( name == "c" ) return LANGUAGE_C;
  if ( name == "c-header" ) return LANGUAGE_C_HEADER;
  if ( name == "cpp-output" ) return LANGUAGE_CPP_OUTPUT;
  if ( name == "assembler" ) return LANGUAGE_ASSEMBLER;

  throw runtime_error( "unknown language name" );
}

GCCStage language_to_stage( const Language lang )
{
  switch ( lang ) {
  case LANGUAGE_C: return PREPROCESS;
  case LANGUAGE_C_HEADER: return PREPROCESS;
  case LANGUAGE_CPP_OUTPUT: return COMPILE;
  case LANGUAGE_ASSEMBLER: return ASSEMBLE;
  case LANGUAGE_OBJECT: return LINK;

  default: throw runtime_error( "unknown language" );
  }
}

string stage_output_name( const GCCStage stage, const string filename )
{
  /* XXX this needs serious attention in the future */
  string result = filename;
  const auto pos_dot = filename.find_last_of( '.' );
  const auto pos_slash = filename.find_last_of( '/' );

  if ( pos_slash == string::npos or pos_dot > pos_slash ) {
    result = result.substr( 0, pos_dot );
  }

  switch ( stage ) {
  case PREPROCESS: return result + ".i";
  case COMPILE: return result + ".s";
  case ASSEMBLE: return result + ".o";

  case NOT_SET:
  case LINK:
    return "a.out";
  }
}

void prepare_args_for_compile( vector<string> & args, const string & output )
{
  /* For compile (.i to .s) we need -S flag. In this function we make sure that
     there are no extra flags (e.g. -E, -c) in the arguments. We also fix the
     output file name. */

  args.erase(
    remove_if(
      args.begin(), args.end(),
      []( const string & s ) { return ( s == "-E" or s == "-c" ); }
    ), args.end()
  );

  bool found_output_flag = false;

  for ( size_t i = 0; i < args.size(); i++ ) {
    if ( args[ i ] == "-o" ) {
      if ( i + 1 == args.size() ) {
        throw runtime_error( "invalid argument: -o option with no argument" );
      }

      args[ i++ ] = output;
      found_output_flag = true;
    }
  }

  if ( not found_output_flag ) {
    args.push_back( "-o" );
    args.push_back( output );
  }
}

void prepare_args_for_assemble( vector<string> & args, const string & output )
{
  /* For assemble (.s to .o) we need -c flag. In this function we make sure that
     there are no extra flags (e.g. -E, -S) in the arguments. We also fix the
     output file name. */

  args.erase(
    remove_if(
      args.begin(), args.end(),
      []( const string & s ) { return ( s == "-E" or s == "-S" ); }
    ), args.end()
  );

  bool found_output_flag = false;

  for ( size_t i = 0; i < args.size(); i++ ) {
    if ( args[ i ] == "-o" ) {
      if ( i + 1 == args.size() ) {
        throw runtime_error( "invalid argument: -o option with no argument" );
      }

      args[ i++ ] = output;
      found_output_flag = true;
    }
  }

  if ( not found_output_flag ) {
    args.push_back( "-o" );
    args.push_back( output );
  }
}

int main( int argc, char * argv[] )
{
  Language current_langauge = LANGUAGE_NONE; /* -x arugment */
  GCCStage last_stage = NOT_SET;

  vector<string> args;

  for ( int i = 0; i < argc; i++ ) {
    args.push_back( argv[ i ] );
  }

  string last_stage_output_filename {};

  vector<pair<string, Language>> input_files;

  const option gcc_options[] = {
    { NULL, required_argument, NULL, 'x' }, /* Specify explicitly the language for the following input files */

    { NULL, no_argument,       NULL, 'E' }, /* Stop after the preprocessing stage */
    { NULL, no_argument,       NULL, 'S' }, /* Stop after the stage of compilation proper */
    { NULL, no_argument,       NULL, 'c' }, /* Compile or assemble the source files, but do not link */

    { NULL, required_argument, NULL, 'o' }, /*  Output file name */

    { 0, 0, 0, 0 },
  };

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

      if ( current_langauge == LANGUAGE_NONE ) {
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
      last_stage = ( last_stage == NOT_SET ) ? PREPROCESS : last_stage;
      break;

    case 'S':
      last_stage = ( last_stage == NOT_SET ) ? COMPILE : last_stage;
      break;

    case 'c':
      last_stage = ( last_stage == NOT_SET ) ? ASSEMBLE : last_stage;
      break;

    case 'o':
      last_stage_output_filename = optarg;
      break;
    }
  }

  if ( input_files.size() == 0 ) {
    throw runtime_error( "no input files" );
  }

  if ( input_files.size() > 1 ) {
    throw runtime_error( "accepting multiple input files is not supported yet" );
  }

  if ( last_stage == NOT_SET ) {
    last_stage = LINK;
  }

  const auto & input = input_files.back();

  const size_t input_idx = distance( args.begin(), find( args.begin(), args.end() , input.first ) );
  assert( input_idx < args.size() );

  GCCStage first_stage = language_to_stage( input.second );

  if ( last_stage_output_filename.length() == 0 ) {
    switch ( last_stage ) {
    case LINK:
      last_stage_output_filename = "a.out";
      break;

    default:
      last_stage_output_filename = stage_output_name( last_stage, input.first );
    }
  }

  assert( first_stage > NOT_SET );

  /* stage -> output_name */
  map<size_t, string> stage_output;
  stage_output[ first_stage - 1 ] = input.first;

  for ( size_t stage = first_stage; stage <= last_stage; stage++ ) {
    string output_name = ( stage == last_stage ) ? last_stage_output_filename
                                                 : stage_output_name( static_cast<GCCStage>( stage ), input.first );

    switch ( stage ) {
    case PREPROCESS:
      /* generate preprocess thunk */
      cerr << ">> preprocessing " << input.first;
      break;

    case COMPILE:
    {
      /* generate compile thunk */
      cerr << ">> compiling " << input.first;

      vector<string> args_compile = args;
      args_compile[ input_idx ] = stage_output[ stage - 1 ];
      prepare_args_for_compile( args_compile, output_name );

      GGModelCompile compile_model( args_compile );
      // compile_model.write_thunk();

      break;
    }

    case ASSEMBLE:
    {
      /* generate assemble thunk */
      cerr << ">> assembling " << input.first;

      vector<string> args_assemble = args;
      args_assemble[ input_idx ] = stage_output[ stage - 1 ];
      prepare_args_for_assemble( args_assemble, output_name );

      GGModelAssemble assemble_model( args_assemble );
      // assemble_model.write_thunk();

      break;
    }

    case LINK:
      /* generate link thunk */
      cerr << ">> linking " << input.first;
      break;
    }

    stage_output[ stage ] = output_name;

    cerr << ", output=" << output_name << endl;
  }

  return 0;
}
