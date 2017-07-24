/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "thunk.hh"

#include <iostream>
#include <stdexcept>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <iomanip>

#include "digest.hh"

using namespace std;
using namespace gg;
using namespace gg::thunk;

const size_t PATH_MAX_LEN = 128;

Function::Function( const string & exe, const vector<string> & args,
                    const vector<string> & envars )
  : exe_( exe ), args_( args ),
    exe_hash_( digest::SHA256( exe_ ).hexdigest() ), envars_( envars )
{}

Function::Function( const string & exe, const vector<string> & args,
                    const vector<string> & envars, const string & hash )
  : exe_( exe ), args_( args ), exe_hash_( hash ), envars_( envars )
{}

Function::Function( const protobuf::Function & func_proto )
  : exe_( func_proto.exe() ),
    args_( func_proto.args().begin(), func_proto.args().end() ),
    exe_hash_( func_proto.hash() ), envars_( func_proto.envars().begin(), func_proto.envars().end() )
{}

protobuf::Function Function::to_protobuf() const
{
  protobuf::Function func;

  func.set_exe( exe_ );

  for ( const string & arg : args_ ) {
    func.add_args( arg );
  }

  func.set_hash( exe_hash_ );

  for ( const string & envar : envars_ ) {
    func.add_envars( envar );
  }

  return func;
}

bool Function::operator==( const Function & other ) const
{
  return ( exe_ == other.exe_ ) and
         ( args_ == other.args_ ) and
         ( exe_hash_ == other.exe_hash_ ) and
         ( envars_ == other.envars_ );
}
