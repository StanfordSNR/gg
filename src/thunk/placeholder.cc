/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "placeholder.hh"

#include <fstream>
#include <iostream>
#include "path.hh"

using namespace std;

const string SHEBANG_DIRECTIVE { "#!/usr/bin/env gg-reduce" };

ThunkPlaceholder::ThunkPlaceholder( const string & hash,
                                    const size_t order,
                                    const off_t size,
                                    const bool execute_after_force )

  : content_hash_( hash ), order_( order ), size_( size ),
    execute_after_force_( execute_after_force )
{}

void ThunkPlaceholder::write( const string & filename ) const
{
  ofstream fout { filename };
  fout << SHEBANG_DIRECTIVE
       << endl
       << content_hash_ << " "
       << order_ << " "
       << size_ << " "
       << execute_after_force_
       << endl;

  roost::chmod( filename, 0755 );
}

Optional<ThunkPlaceholder> ThunkPlaceholder::read( const string & filename )
{
  bool execute_after_force = false;

  ifstream fin { filename };
  string line;
  getline( fin, line );

  if ( line != SHEBANG_DIRECTIVE ) {
    return {};
  }

  string hash;
  size_t order;
  off_t size;

  fin >> hash >> order >> size >> execute_after_force;

  return ThunkPlaceholder( hash, order, size, execute_after_force );
}
