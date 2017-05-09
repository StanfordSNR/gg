/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "thunk_func.hh"

#include <iostream>
#include <stdexcept>
#include <unistd.h>
#include <openssl/sha.h>
#include <fstream>
#include <sstream>
#include <iomanip>

using namespace std;
using namespace gg;

const size_t PATH_MAX_LEN = 128;

ThunkFunc::ThunkFunc( const vector<string> & cmd )
  : exe_( get_exe_path( cmd[0] ) ), args_( cmd ), exe_hash_( hash_exe( exe_ ) )
{
  // TODO : Remove print statement
  for ( auto & c : cmd ) {
    cout << c << " ";
  }
  cout << endl;
}

ThunkFunc::ThunkFunc( const protobuf::Function & func_proto )
  : exe_( func_proto.exe() ), args_( func_proto.args().begin(), func_proto.args().end() ),
    exe_hash_( func_proto.hash() )
{}

string ThunkFunc::get_exe_path( string exe ) {
  // TODO : Implement this for real
  return "/usr/bin/" + exe;
}

string ThunkFunc::hash_exe( string exe ){
  std::ifstream file( exe, std::ifstream::binary );
  SHA256_CTX md5Context;
  SHA256_Init( &md5Context );
  char buf[1024 * 16];
  while ( file.good() ) {
    file.read( buf, sizeof( buf ) );
    SHA256_Update( &md5Context, buf, file.gcount() );
  }
  unsigned char result[ SHA256_DIGEST_LENGTH ];
  SHA256_Final( result, &md5Context );

  // TODO : Consider using a different object than string
  std::stringstream md5string;
  md5string << std::hex << std::uppercase << std::setfill('0');
  for( const auto &byte: result ){
    md5string << std::setw( 2 ) << ( int )byte;
  }
  return md5string.str();
}

protobuf::Function ThunkFunc::to_protobuf() const
{
  protobuf::Function func;

  func.set_exe( exe_ );

  for ( const string & arg : args_ ) {
    func.add_args( arg );
  }

  func.set_hash( exe_hash_ );

  return func;
}
