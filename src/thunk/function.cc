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

Function::Function( const vector<string> & cmd )
  : exe_( get_exe_path( cmd[0] ) ), args_( cmd ), exe_hash_( hash_exe( exe_ ) )
{
  // TODO : Remove print statement
  for ( auto & c : cmd ) {
    cout << c << " ";
  }
  cout << endl;
}

Function::Function( const std::vector<std::string> & cmd, const std::string & hash )
  : exe_( get_exe_path( cmd[0] ) ), args_( cmd ), exe_hash_( hash )
{}

Function::Function( const protobuf::Function & func_proto )
  : exe_( func_proto.exe() ), args_( func_proto.args().begin(), func_proto.args().end() ),
    exe_hash_( func_proto.hash() )
{}

string Function::get_exe_path( const string & exe )
{
  // TODO : Implement this for real
  return exe;
}

string Function::hash_exe( const string & exe )
{
  std::ifstream file( exe, std::ifstream::binary );
  return digest::SHA256( file ).hexdigest();
}

protobuf::Function Function::to_protobuf() const
{
  protobuf::Function func;

  func.set_exe( exe_ );

  for ( const string & arg : args_ ) {
    func.add_args( arg );
  }

  func.set_hash( exe_hash_ );

  return func;
}

bool Function::operator==( const Function & other ) const
{
  return ( exe_ == other.exe_ ) and
         ( args_ == other.args_ ) and
         ( exe_hash_ == other.exe_hash_ );
}
