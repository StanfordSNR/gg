/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "placeholder.hh"

#include <fstream>
#include <sstream>
#include <iostream>
#include <regex>

#include "util/path.hh"

using namespace std;

static const std::string SHEBANG_DIRECTIVE { "#!/usr/bin/env gg-force-and-run" };
static const std::string LIBRARY_DIRECTIVE { "OUTPUT_FORMAT(\"elf64-x86-64\")/*" };

ThunkPlaceholder::ThunkPlaceholder( const string & hash, const string & metadata )
  : content_hash_( hash ), metadata_( metadata )
{}

string ThunkPlaceholder::str( const Type type ) const
{
  const string & header = (type == Type::LinkerScript)
    ? LIBRARY_DIRECTIVE
    : SHEBANG_DIRECTIVE;

  ostringstream sout { ios::out | ios::binary };
  sout << header << endl
       << content_hash_ << endl
       << metadata_
       << ( type == Type::LinkerScript ? "*/" : "" );

  return sout.str();
}

void ThunkPlaceholder::write( const string & filename ) const
{
  regex so_pattern { ".+\\.so[\\.\\d+]+$" };
  smatch match;

  /* guess placeholder type based on filename extension */
  const auto pos = filename.find_last_of( '.' );
  const string extension = filename.substr( pos + 1 );
  if ( extension == "so" or extension == "a" or extension == "o" or
       extension == "s" or extension == "S" or extension == "sho" or
       regex_match( filename, match, so_pattern ) ) {
    write( filename, Type::LinkerScript );
  } else {
    write( filename, Type::ShellScript );
  }
}

void ThunkPlaceholder::write( const string & filename, const Type type ) const
{
  roost::atomic_create( str( type ), filename );

  if ( type == Type::ShellScript ) {
    roost::chmod( filename, 0755 );
  }
}

Optional<ThunkPlaceholder> ThunkPlaceholder::read( const string & filename )
{
  ifstream fin { filename, ios::in | ios::binary };
  string line;
  getline( fin, line );

  Type type;

  if ( line == SHEBANG_DIRECTIVE ) {
    type = Type::ShellScript;
  }
  else if ( line == LIBRARY_DIRECTIVE ) {
    type = Type::LinkerScript;
  }
  else {
    return {};
  }

  string hash;
  fin >> hash;

  if ( not fin.good() ) {
    throw runtime_error( "failed reading from " + filename );
  }

  /* finish the line */
  getline( fin, line );

  /* now let's see if there's metadata */
  string metadata;
  char read_buffer[ 1024 * 1024 ];

  while ( true ) {
    fin.read( read_buffer, sizeof( read_buffer ) );
    metadata.append( read_buffer, fin.gcount() );
    if ( not fin.good() ) {
      break;
    }
  }

  if ( type == Type::LinkerScript and metadata.length() >= 2 ) {
    metadata.pop_back();
    metadata.pop_back();
  }

  return ThunkPlaceholder { hash, metadata };
}

bool ThunkPlaceholder::is_placeholder( FileDescriptor && fd )
{
  assert( SHEBANG_DIRECTIVE.length() == LIBRARY_DIRECTIVE.length() );
  const string header = fd.read_exactly( SHEBANG_DIRECTIVE.length(), true );
  return (header == SHEBANG_DIRECTIVE) or (header == LIBRARY_DIRECTIVE);
}
