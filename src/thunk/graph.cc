/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "graph.hh"

#include <stdexcept>

#include "ggpaths.hh"
#include "thunk_reader.hh"

using namespace std;
using namespace gg::thunk;

DependencyGraph::DependencyGraph()
{}

void DependencyGraph::add_thunk( const string & hash )
{
  if ( thunks_.count( hash ) > 0 ) {
    return; // already added
  }

  ThunkReader thunk_reader { gg::paths::blob_path( hash ).string() };
  Thunk thunk = thunk_reader.read_thunk();

  for ( const InFile & infile : thunk.infiles() ) {
    if ( infile.order() > 0 ) {
      add_thunk( infile.content_hash() );
      referenced_thunks_[ infile.content_hash() ].insert( hash );
    }
  }

  thunks_.emplace( make_pair( hash, move( thunk ) ) );
}


unordered_set<string> DependencyGraph::force_thunk( const string & old_hash,
                                                    const string & new_hash )
{
  unordered_set<string> order_one_thunks;

  const Thunk & old_thunk = thunks_.at( old_hash );

  if ( old_thunk.order() != 1 ) {
    throw runtime_error( "can't force thunks with order != 1" );
  }

  if ( referenced_thunks_.count( old_hash ) == 0 ) {
    // no other thunk actually referenced this thunk
    // XXX is this the right thing to do?
    return order_one_thunks;
  }

  for ( const string & thash : referenced_thunks_.at( old_hash ) ) {
    Thunk & ref_thunk = thunks_.at( thash );
    ref_thunk.update_infile( old_hash, new_hash, 0, 0 );

    if ( ref_thunk.order() == 1 ) {
      order_one_thunks.insert( thash );
    }
  }

  return order_one_thunks;
}

unordered_set<string>
DependencyGraph::order_one_dependencies( const string & thunk_hash )
{
  unordered_set<string> result;
  const Thunk & thunk = get_thunk( thunk_hash );

  if ( thunk.order() == 1 ) {
    result.insert( thunk_hash );
  }

  for ( const InFile & infile : thunk.infiles() ) {
    if ( infile.order() == 1 ) {
      result.insert( infile.content_hash() );
    }
    else if ( infile.order() > 1 ) {
      unordered_set<string> subresult = order_one_dependencies( infile.content_hash() );
      result.insert( subresult.begin(), subresult.end() );
    }
  }

  return result;
}
