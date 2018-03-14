/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "gcc.hh"

#include <string>
#include <vector>
#include <stdexcept>
#include <unordered_map>
#include <iostream>
#include <regex>

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
  { "S",   Language::ASSEMBLER_WITH_CPP },
  { "sx",  Language::ASSEMBLER_WITH_CPP },
  { "o",   Language::OBJECT },
  { "so",  Language::SHARED_OBJECT },
  { "sho", Language::OBJECT },
  { "lo",  Language::OBJECT },
  { "a",   Language::ARCHIVE_LIBRARY },
};

const unordered_map<string, Language> name_to_lang = {
  { "none",               Language::NONE },
  { "c",                  Language::C },
  { "c-header",           Language::C_HEADER },
  { "cpp-output",         Language::CPP_OUTPUT },
  { "c++",                Language::CXX },
  { "c++-header",         Language::CXX_HEADER },
  { "c++-cpp-output",     Language::CXX_CPP_OUTPUT },
  { "assembler",          Language::ASSEMBLER },
  { "assembler-with-cpp", Language::ASSEMBLER_WITH_CPP },
};

const unordered_map<Language, string> lang_to_name = {
  { Language::NONE,               "none" },
  { Language::C,                  "c" },
  { Language::C_HEADER,           "c-header" },
  { Language::CPP_OUTPUT,         "cpp-output" },
  { Language::CXX,                "c++" },
  { Language::CXX_HEADER,         "c++-header" },
  { Language::CXX_CPP_OUTPUT,     "c++-cpp-output" },
  { Language::ASSEMBLER,          "assembler" },
  { Language::ASSEMBLER_WITH_CPP, "assembler-with-cpp" },
};

/* split an input filename (e.g. "hello.abc.c") into the base ("hello.abc")
   and the last extension (e.g. "c") */
pair<string, string> GCCModelGenerator::split_source_name( const string & path )
{
  vector<char> path_cstr( path.c_str(), path.c_str() + path.size() + 1 );
  const string filename { basename( path_cstr.data() ) };
  const auto pos = filename.find_last_of( '.' );

  if ( pos == string::npos ) {
    /* did not find a file extension! */
    throw runtime_error( "missing extension in filename: " + path );
  }

  return { filename.substr( 0, pos ), filename.substr( pos + 1 ) };
}

Language GCCModelGenerator::filename_to_language( const std::string & path )
{
  string extension = split_source_name( path ).second;

  if ( ext_to_lang.count( extension ) > 0 ) {
    return ext_to_lang.at( extension );
  }

  regex so_pattern { ".+\\.so[\\.\\d+]+$" };
  smatch match;
  regex_match( path, match, so_pattern );

  if ( match.size() > 0 ) {
    return Language::SHARED_OBJECT;
  }

  throw runtime_error( "unknown file extension: " + path );
}

Language GCCModelGenerator::name_to_language( const string & name )
{
  if ( name_to_lang.count( name ) > 0 ) {
    return name_to_lang.at( name );
  }

  throw runtime_error( "unknown language name: " + name );
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
  case Language::ASSEMBLER_WITH_CPP:
    return PREPROCESS;

  case Language::CPP_OUTPUT:
  case Language::CXX_CPP_OUTPUT:
    return COMPILE;

  case Language::ASSEMBLER:
    return ASSEMBLE;

  case Language::OBJECT:
  case Language::SHARED_OBJECT:
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

void error_if( const char * name )
{
  if ( getenv( name ) ) {
    throw runtime_error( "unsupported environment variable: " + string( name ) );
  }
}

string environment_or_blank( const string & name )
{
  const char * value = getenv( name.c_str() );
  if ( value ) {
    return name + "=" + value;
  } else {
    return name + "=";
  }
}

void verify_no_unhandled_environment_variables()
{
  error_if( "CPATH" );
  error_if( "C_INCLUDE_PATH" );
  error_if( "CPLUS_INCLUDE_PATH" );
  error_if( "OBJC_INCLUDE_PATH" );
  error_if( "DEPENDENCIES_OUTPUT" );
  error_if( "SUNPRO_DEPENDENCIES" );
  error_if( "SOURCE_DATE_EPOCH" );
  error_if( "GCC_EXEC_PREFIX" );
  error_if( "COMPILER_PATH" );
  error_if( "LIBRARY_PATH" );
}

vector<string> GCCModelGenerator::gcc_environment()
{
  verify_no_unhandled_environment_variables();

  vector<string> environment;
  environment.push_back( environment_or_blank( "LANG" ) );
  environment.push_back( environment_or_blank( "LC_CTYPE" ) );
  environment.push_back( environment_or_blank( "LC_MESSAGES" ) );
  environment.push_back( environment_or_blank( "LC_ALL" ) );
  return environment;
}
