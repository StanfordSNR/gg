/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "graph.hh"

#include <stdexcept>

#include "paths.hh"
#include "thunk_reader.hh"

using namespace std;
using namespace gg::thunk;

DependencyGraph::DependencyGraph()
{}

size_t DependencyGraph::add_thunk( const string & hash )
{
  if ( hash_to_id_.count( hash ) > 0 ) {
    const size_t thunk_id = hash_to_id_[ hash ];
    if ( thunks_.count( thunk_id ) > 0 ) {
      return thunk_id; // already added
    }
    else {
      throw runtime_error( "cycle detected while adding thunk: " + hash );
    }
  }

  ThunkReader thunk_reader { gg::paths::blob_path( hash ).string() };
  Thunk thunk = thunk_reader.read_thunk();

  const size_t thunk_id = insert_thunk_hash( hash );

  for ( const InFile & infile : thunk.infiles() ) {
    if ( infile.order() > 0 ) {
      size_t infile_thunk_id = add_thunk( infile.content_hash() );
      referenced_thunks_[ infile_thunk_id ].insert( thunk_id );
    }
  }

  thunks_.emplace( make_pair( thunk_id, move( thunk ) ) );

  return thunk_id;
}

size_t DependencyGraph::insert_thunk_hash( const string & hash )
{
  unique_lock<mutex> lock { unique_id_mutex_ };

  static size_t auto_increment_id = 0;
  hash_to_id_.insert( { hash, auto_increment_id } );
  id_to_hash_.insert( { auto_increment_id, hash } );
  return auto_increment_id++;
}
