/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "placeholder.hh"

#include <fstream>
#include <iostream>
#include "path.hh"

using namespace std;

ThunkPlaceholder::ThunkPlaceholder( const string & hash,
                                    const size_t order,
                                    const off_t size )

  : content_hash_( hash ), order_( order ), size_( size )
{}

void ThunkPlaceholder::write( const string & filename ) const
{
  ofstream fout { filename };
  fout << SHEBANG_DIRECTIVE
       << endl
       << content_hash_ << " "
       << order_ << " "
       << size_
       << endl;

  roost::chmod( filename, 0755 );
}

Optional<ThunkPlaceholder> ThunkPlaceholder::read( const string & filename )
{
  ifstream fin { filename };
  string line;
  getline( fin, line );

  if ( line != SHEBANG_DIRECTIVE ) {
    return {};
  }

  string hash;
  size_t order;
  off_t size;

  fin >> hash >> order >> size;

  return ThunkPlaceholder { hash, order, size };
}

bool ThunkPlaceholder::is_placeholder( FileDescriptor && fd )
{
  string shebang = fd.read_exactly( SHEBANG_DIRECTIVE.length(), true );
  return shebang == SHEBANG_DIRECTIVE;
}
