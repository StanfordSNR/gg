/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "model-gcc.hh"

#include <string>
#include <vector>
#include <stdexcept>
#include <unordered_map>
#include <iostream>

using namespace std;

const unordered_map<string, Language> ext_to_lang = {
  { "c",   Language::C },
  { "h",   Language::C_HEADER },
  { "i",   Language::CPP_OUTPUT },
  { "C",   Language::CXX },
  { "cc",  Language::CXX },
  { "cxx", Language::CXX },
  { "cpp", Language::CXX },
  { "CPP", Language::CXX },
  { "c++", Language::CXX },
  { "ii",  Language::CXX_CPP_OUTPUT },
  { "s",   Language::ASSEMBLER },
  { "o",   Language::OBJECT },
  { "a",   Language::ARCHIVE_LIBRARY },
};

const unordered_map<string, Language> name_to_lang = {
  { "none",           Language::NONE },
  { "c",              Language::C },
  { "c-header",       Language::C_HEADER },
  { "cpp-output",     Language::CPP_OUTPUT },
  { "c++",            Language::CXX },
  { "c++-header",     Language::CXX_HEADER },
  { "c++-cpp-output", Language::CXX_CPP_OUTPUT },
  { "assembler",      Language::ASSEMBLER },
};

const unordered_map<Language, string> lang_to_name = {
  { Language::NONE,           "none" },
  { Language::C,              "c" },
  { Language::C_HEADER,       "c-header" },
  { Language::CPP_OUTPUT,     "cpp-output" },
  { Language::CXX,            "c++" },
  { Language::CXX_HEADER,     "c++-header" },
  { Language::CXX_CPP_OUTPUT, "c++-cpp-output" },
  { Language::ASSEMBLER,      "assembler" },
};

Language GCCModelGenerator::filename_to_language( const string & path )
{
  vector<char> path_cstr( path.c_str(), path.c_str() + path.size() + 1 );
  const string filename { basename( path_cstr.data() ) };
  const auto pos = filename.find_last_of( '.' );

  if ( pos == string::npos ) {
    /* did not find a file extension! */
  }

  string extension = filename.substr( pos + 1 );

  if ( ext_to_lang.count( extension ) > 0 ) {
    return ext_to_lang.at( extension );
  }

  throw runtime_error( "unknown file extension" );
}

Language GCCModelGenerator::name_to_language( const string & name )
{
  if ( name_to_lang.count( name ) > 0 ) {
    return name_to_lang.at( name );
  }

  throw runtime_error( "unknown language name" );
}

string GCCModelGenerator::language_to_name( const Language & lang )
{
  if ( lang_to_name.count( lang ) > 0 ) {
    return lang_to_name.at( lang );
  }

  throw runtime_error( "unknown language" );
}

GCCStage GCCModelGenerator::language_to_stage( const Language lang )
{
  switch ( lang ) {
  case Language::C:
  case Language::CXX:
  case Language::C_HEADER:
  case Language::CXX_HEADER:
    return PREPROCESS;

  case Language::CPP_OUTPUT:
  case Language::CXX_CPP_OUTPUT:
    return COMPILE;

  case Language::ASSEMBLER:
    return ASSEMBLE;

  case Language::OBJECT:
    return LINK;

  default: throw runtime_error( "unknown language" );
  }
}

string GCCModelGenerator::stage_output_name( const GCCStage stage, const string basename )
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
