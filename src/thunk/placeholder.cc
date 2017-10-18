/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "placeholder.hh"

#include <fstream>
#include <iostream>
#include "path.hh"

using namespace std;

static const std::string SHEBANG_DIRECTIVE { "#!/usr/bin/env gg-force-and-run" };
static const std::string LIBRARY_DIRECTIVE { "OUTPUT_FORMAT(\"elf64-x86-64\") /*" };

ThunkPlaceholder::ThunkPlaceholder( const string & hash,
                                    const size_t order,
                                    const off_t size )

  : content_hash_( hash ), order_( order ), size_( size )
{}

void ThunkPlaceholder::write( const string & filename ) const
{
  /* guess placeholder type based on filename extension */
  const auto pos = filename.find_last_of( '.' );
  const string extension = filename.substr( pos + 1 );
  if ( extension == "so" or extension == "a" or extension == "o"
       or extension == "s" or extension == "S" ) {
    write( filename, Type::LinkerScript );
  } else {
    write( filename, Type::ShellScript );
  }
}

void ThunkPlaceholder::write( const string & filename, const Type type ) const
{
  const string & header = (type == Type::LinkerScript)
    ? LIBRARY_DIRECTIVE
    : SHEBANG_DIRECTIVE;

  ofstream fout { filename };
  fout << header
       << endl
       << content_hash_ << " "
       << order_ << " "
       << size_
       << "\n" << '*' << '/'
       << endl;

  if ( not fout.good() ) {
    throw runtime_error( "failed writing to " + filename );
  }

  if ( type == Type::ShellScript ) {
    roost::chmod( filename, 0755 );
  }
}

Optional<ThunkPlaceholder> ThunkPlaceholder::read( const string & filename )
{
  ifstream fin { filename };
  string line;
  getline( fin, line );

  if ( line != SHEBANG_DIRECTIVE
       and line != LIBRARY_DIRECTIVE ) {
    return {};
  }

  string hash;
  size_t order;
  off_t size;

  fin >> hash >> order >> size;

  if ( not fin.good() ) {
    throw runtime_error( "failed reading from " + filename );
  }

  return ThunkPlaceholder { hash, order, size };
}

bool ThunkPlaceholder::is_placeholder( FileDescriptor && fd )
{
  assert( SHEBANG_DIRECTIVE.length() == LIBRARY_DIRECTIVE.length() );
  const string header = fd.read_exactly( SHEBANG_DIRECTIVE.length(), true );
  return (header == SHEBANG_DIRECTIVE) or (header == LIBRARY_DIRECTIVE);
}
