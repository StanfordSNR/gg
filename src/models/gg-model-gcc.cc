/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <vector>
#include <string>
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
  LANGUAGE_ASSEMBLER
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

  throw runtime_error( "unknown file extension" );
}

Language name_to_language( const string & name )
{
  if ( name == "c" ) return LANGUAGE_C;
  if ( name == "c-header" ) return LANGUAGE_C_HEADER;
  if ( name == "cpp-output" ) return LANGUAGE_CPP_OUTPUT;
  if ( name == "assembler" ) return LANGUAGE_ASSEMBLER;

  throw runtime_error( "unknown language name" );
}

int main( int argc, char * argv[] )
{
  Language current_langauge = LANGUAGE_NONE; /* -x arugment */
  GCCStage end_stage = NOT_SET;

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
    }

    switch ( opt ) {
    case 'x':
      current_langauge = name_to_language( optarg );
      break;

    case 'E':
      end_stage = ( end_stage == NOT_SET ) ? PREPROCESS : end_stage;
      break;

    case 'S':
      end_stage = ( end_stage == NOT_SET ) ? COMPILE : end_stage;
      break;

    case 'c':
      end_stage = ( end_stage == NOT_SET ) ? ASSEMBLE : end_stage;
      break;
    }
  }

  return 0;
}
