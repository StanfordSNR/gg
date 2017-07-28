/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "model-gcc.hh"

#include <string>
#include <vector>
#include <stdexcept>

using namespace std;

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
