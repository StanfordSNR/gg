/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "thunk_func.hh"

#include <iostream>

using namespace std;

ThunkFunc::ThunkFunc( vector<string> cmd )
  : args_( cmd )
{
  for ( auto it = cmd.begin(); it != cmd.end(); ++it ) {
    cout << *it << " ";
  }

  cout << endl;
}

ThunkFunc::~ThunkFunc()
{}

json::Object ThunkFunc::to_json()
{
  json::Object j;
  j[ "test" ] = json::String( "UNIMPLEMENTED" );
  return j;
}
