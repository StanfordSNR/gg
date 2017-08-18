/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "util.hh"

#include <cstdlib>
#include <string>
#include <stdexcept>

using namespace std;

string safe_getenv( const string & key )
{
  const char * const value = getenv( key.c_str() );
  if ( not value ) {
    throw runtime_error( "missing environment variable: " + key );
  }
  return value;
}
