/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "thunk.hh"

#include <iostream>
#include <stdexcept>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <iomanip>

using namespace std;
using namespace gg;
using namespace gg::thunk;

Function::Function( const string & hash, const vector<string> & args,
                    const vector<string> & envars )
  : hash_( hash ), args_( args ), envars_( envars )
{}

Function::Function( string && hash, vector<string> && args,
                    vector<string> && envars )
  : hash_( move( hash ) ), args_( move( args ) ), envars_( move( envars ) )
{}

Function::Function( const protobuf::Function & func_proto )
  : hash_( func_proto.hash() ),
    args_( func_proto.args().begin(), func_proto.args().end() ),
    envars_( func_proto.envars().begin(), func_proto.envars().end() )
{}

protobuf::Function Function::to_protobuf() const
{
  protobuf::Function func;

  func.set_hash( hash_ );

  for ( const string & arg : args_ ) {
    func.add_args( arg );
  }

  for ( const string & envar : envars_ ) {
    func.add_envars( envar );
  }

  return func;
}

bool Function::operator==( const Function & other ) const
{
  return ( hash_ == other.hash_ ) and
         ( args_ == other.args_ ) and
         ( envars_ == other.envars_ );
}
