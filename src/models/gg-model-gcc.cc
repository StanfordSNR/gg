/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <vector>
#include <string>
#include <tuple>
#include <getopt.h>
#include <libgen.h>

#include "exception.hh"

using namespace std;

enum GCCStage
{
  NOT_SET,
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
  auto const pos = filename.find_last_of( '.' );

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

int main( int argc, char * argv[] )
{
  Language current_langauge = LANGUAGE_NONE; /* -x arugment */
  GCCStage last_stage = NOT_SET;

  vector<pair<string, Language>> input_files;

  const option gcc_options[] = {
    { NULL, required_argument, NULL, 'x' }, /* Specify explicitly the language for the following input files */
    { NULL, no_argument,       NULL, 'E' }, /* Stop after the preprocessing stage */
    { NULL, no_argument,       NULL, 'S' }, /* Stop after the stage of compilation proper */
    { NULL, no_argument,       NULL, 'c' }, /* Compile or assemble the source files, but do not link. */

    { 0, 0, 0, 0 },
  };

  while ( true ) {
    const int opt = getopt_long( argc, argv, "-x:ESc", gcc_options, nullptr );

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

  GCCStage first_stage = language_to_stage( input.second );

  for ( size_t stage = first_stage; stage <= last_stage; stage++ ) {
    switch ( stage ) {
    case PREPROCESS:
      /* generate preprocess thunk */
      cerr << ">> preprocessing " << input.first << endl;
      break;

    case COMPILE:
      /* generate compile thunk */
      cerr << ">> compiling " << input.first << endl;
      break;

    case ASSEMBLE:
      /* generate assemble thunk */
      cerr << ">> assembling " << input.first << endl;
      break;

    case LINK:
      /* generate link thunk */
      cerr << ">> linking " << input.first << endl;
      break;
    }
  }

  return 0;
}
