/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "thunk.hh"

using namespace std;
using namespace gg;

Thunk::Thunk( string outfile, ThunkFunc thunkfunc,
              vector<InFile> infiles )
  : outfile_( outfile ), thunkfunc_( thunkfunc ), infiles_( infiles ),
    order_( 0 )
{}

Thunk::Thunk( const gg::protobuf::Thunk & thunk_proto )
  : outfile_( thunk_proto.outfile() ), thunkfunc_( thunk_proto.function() ),
    infiles_(), order_( 0 )
{
  for ( protobuf::InFile infile : thunk_proto.infiles() ) {
    infiles_.push_back( { infile } );
  }
}

protobuf::Thunk Thunk::to_protobuf() const
{
  protobuf::Thunk thunk;

  thunk.set_outfile( outfile_ );
  *thunk.mutable_function() = thunkfunc_.to_protobuf();

  for ( const InFile & infile : infiles_ ) {
    *thunk.add_infiles() = infile.to_protobuf();
  }

  return thunk;
}
