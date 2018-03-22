/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "graph.hh"

#include <stdexcept>

#include "ggutils.hh"
#include "thunk.hh"
#include "thunk_reader.hh"
#include "thunk_writer.hh"

using namespace std;
using namespace gg::thunk;

void ExecutionGraph::add_thunk( const std::string & hash )
{
  if ( thunks_.count( hash ) ) {
    /* we already have this thunk */
    throw runtime_error( "possible cycle in the execution graph" );
  }

  ThunkReader thunk_reader { gg::paths::blob_path( hash ).string() };
  Thunk thunk { thunk_reader.read_thunk() };
  thunk.set_hash( hash );

  for ( const Thunk::DataItem & item : thunk.values() ) {
    value_dependencies_.emplace( item.first );
  }

  for ( const Thunk::DataItem & item : thunk.executables() ) {
    executable_dependencies_.emplace( item.first );
  }

  for ( const Thunk::DataItem & item : thunk.thunks() ) {
    add_thunk( item.first );
  }

  thunks_.emplace( piecewise_construct,
                   forward_as_tuple( hash ),
                   forward_as_tuple( move( thunk ) ) );
}
