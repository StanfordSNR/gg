/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "thunk.hh"

using namespace std;

Thunk::Thunk( string outfile, ThunkFunc thunkfunc,
              vector<InFileDescriptor> infiles )
  : outfile_( outfile ), thunkfunc_( thunkfunc ), infiles_( infiles ),
    order_( 0 )
{}

Thunk::~Thunk()
{}

json::Object Thunk::to_json()
{
  json::Object obj;

  obj[ "outfile" ] = json::String( outfile_ );
  obj[ "function" ] = thunkfunc_.to_json();

  json::Array j_infiles;
  for ( auto it = infiles_.begin(); it != infiles_.end(); ++it ) {
      j_infiles.Insert( it->to_json() );
  }

  obj[ "infiles" ] = j_infiles;

  return obj;
}

string Thunk::get_outfile() {
  return outfile_;
}
