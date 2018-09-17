/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "placeholder.hh"

#include <fstream>
#include <sstream>
#include <iostream>
#include <regex>

#include "util/path.hh"

using namespace std;

static constexpr char SHEBANG_DIRECTIVE[] = "#!/usr/bin/env gg-force-and-run";
static constexpr char LIBRARY_DIRECTIVE[] = "OUTPUT_FORMAT(\"elf64-x86-64\")/*";

ThunkPlaceholder::ThunkPlaceholder( const string & hash )
  : content_hash_( hash )
{}

string ThunkPlaceholder::str( const Type type ) const
{
  const char * header = (type == Type::LinkerScript)
    ? LIBRARY_DIRECTIVE
    : SHEBANG_DIRECTIVE;

  ostringstream sout { ios::out | ios::binary };
  sout << header << endl
       << content_hash_ << endl
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
  char buffer[ sizeof( SHEBANG_DIRECTIVE ) ];

  ifstream fin { filename, ios::in | ios::binary };
  fin.read( buffer, sizeof( SHEBANG_DIRECTIVE ) );

  if ( strncmp( buffer, SHEBANG_DIRECTIVE, sizeof( SHEBANG_DIRECTIVE ) - 1 ) != 0 and
       strncmp( buffer, LIBRARY_DIRECTIVE, sizeof( LIBRARY_DIRECTIVE ) - 1 ) != 0 ) {
    return {};
  }

  string hash;
  fin >> hash;

  if ( not fin.good() ) {
    throw runtime_error( "failed reading from " + filename );
  }

  return ThunkPlaceholder { hash };
}

bool ThunkPlaceholder::is_placeholder( FileDescriptor && fd )
{
  static_assert( sizeof( SHEBANG_DIRECTIVE ) == sizeof( LIBRARY_DIRECTIVE ),
                 "length of shebang directive and library directive must be equal" );

  const string header = fd.read_exactly( sizeof( SHEBANG_DIRECTIVE ) - 1, true );
  return ( header.compare( 0, sizeof( SHEBANG_DIRECTIVE ) - 1, SHEBANG_DIRECTIVE ) == 0 ) or
         ( header.compare( 0, sizeof( LIBRARY_DIRECTIVE ) - 1, LIBRARY_DIRECTIVE ) == 0 );
}
