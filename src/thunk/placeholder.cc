/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "placeholder.hh"

#include <fstream>
#include <iostream>
#include "path.hh"

using namespace std;

const string SHEBANG_DIRECTIVE { "#!/usr/bin/env gg-reduce" };
const string EXEC_FLAG { " -e" };

ThunkPlaceholder::ThunkPlaceholder( const string & hash,
                                    const bool execute_after_force )

  : hash_( hash ), execute_after_force_( execute_after_force )
{}

void ThunkPlaceholder::write( const string & filename ) const
{
  ofstream fout { filename };
  fout << SHEBANG_DIRECTIVE
       << ( execute_after_force_ ? EXEC_FLAG : string( "" ) )
       << endl
       << hash_
       << endl;

  roost::chmod( filename, 755 );
}

Optional<ThunkPlaceholder> ThunkPlaceholder::read( const string & filename )
{
  string hash;
  bool execute_after_force = false;

  ifstream fin { filename };
  string line;
  getline( fin, line );

  if ( line == ( SHEBANG_DIRECTIVE + EXEC_FLAG ) ) {
    execute_after_force = true;
  }
  else if ( line != SHEBANG_DIRECTIVE ) {
    return {};
  }

  getline( fin, hash );
  return ThunkPlaceholder( line, execute_after_force );
}
