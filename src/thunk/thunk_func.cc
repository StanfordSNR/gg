/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <iostream>
#include <stdexcept>
#include <unistd.h>
#include <openssl/sha.h>
#include <fstream>
#include <sstream>
#include <iomanip>

#include "thunk_func.hh"


using namespace std;

const size_t PATH_MAX_LEN = 128;

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

ThunkFunc::ThunkFunc( const vector<string> & cmd )
  : exe_( get_exe_path( cmd[0] ) ), args_( cmd ), exe_hash_( hash_exe( exe_ ) )
{
  // TODO : Remove print statement
  for ( auto & c : cmd ) {
    cout << c << " ";
  }
  cout << endl;
}

ThunkFunc::~ThunkFunc()
{}

json::Object ThunkFunc::to_json()
{
  json::Object j;
  j[ "exe" ] = json::String( exe_ );
  json::Array jargs;
  for(auto it = args_.begin(); it != args_.end(); ++it){
    jargs.Insert(json::String(*it));
  }
  j[ "args" ] = jargs;
  j[ "hash" ] = json::String( exe_hash_ );
  return j;
}
