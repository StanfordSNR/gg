/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "graph.hh"

#include <stdexcept>

#include "thunk/ggutils.hh"
#include "thunk/thunk_reader.hh"
#include "thunk/thunk_writer.hh"

using namespace std;
using namespace gg::thunk;

void DependencyGraph::add_thunk( const string & hash )
{
  if ( thunks_.count( hash ) > 0 ) {
    return; // already added
  }

  ThunkReader thunk_reader { gg::paths::blob_path( hash ).string() };
  Thunk thunk = thunk_reader.read_thunk();
  thunk.set_hash( hash );

  for ( const string & h : thunk.data_values() ) {
    value_dependencies_.insert( h );
  }

  for ( const string & h : thunk.executables() ) {
    executable_dependencies_.insert( h );
  }

  for ( const string & h : thunk.data_thunks() ) {
    add_thunk( h );
    referenced_thunks_[ h ].insert( hash );
  }

  thunks_.emplace( make_pair( hash, move( thunk ) ) );
}

void DependencyGraph::update_thunk_hash( const string & old_hash,
                                         const string & new_hash )
{
  assert( thunks_.at( old_hash ).executable() );

  thunks_.emplace( make_pair( new_hash, move( thunks_.at( old_hash ) ) ) );
  thunks_.erase( old_hash );

  if ( original_hashes_.count( old_hash ) == 0 ) {
    updated_hashes_.insert( { old_hash, new_hash } );
    original_hashes_.insert( { new_hash, old_hash } );
  }
  else {
    updated_hashes_[ original_hashes_[ old_hash ] ] = new_hash;
    original_hashes_.erase( old_hash );
  }

  if ( referenced_thunks_.count( old_hash ) ) {
    referenced_thunks_.emplace( make_pair( new_hash, move( referenced_thunks_.at( old_hash ) ) ) );
    referenced_thunks_.erase( old_hash );

    for ( const string & thash : referenced_thunks_.at( new_hash ) ) {
      Thunk & ref_thunk = thunks_.at( thash );
      ref_thunk.update_data( old_hash, new_hash );
    }
  }
}

Optional<unordered_set<string>> DependencyGraph::force_thunk( const string & old_hash,
                                                              const string & new_hash )
{
  unordered_set<string> order_one_thunks;

  if ( not thunks_.count( old_hash ) ) {
    /* we already have forced this thunk, or maybe we never had it in the first
    place. */
    return { false };
  }

  if ( thunks_.at( old_hash ).executable() ) {
    throw runtime_error( "can't force thunks with unresolved dependencies" );
  }

  /* we don't need this thunk anymore, so let's just delete it from memory */
  thunks_.erase( old_hash );

  if ( referenced_thunks_.count( old_hash ) == 0 ) {
    // no other thunk actually referenced this thunk
    // XXX is this the right thing to do?
    return order_one_thunks;
  }

  for ( const string & thash : referenced_thunks_.at( old_hash ) ) {
    Thunk & ref_thunk = thunks_.at( thash );
    ref_thunk.update_data( old_hash, new_hash );

    if ( ref_thunk.executable() ) {
      const string ref_thunk_hash = ThunkWriter::write_thunk( ref_thunk );
      update_thunk_hash( thash, ref_thunk_hash );
      order_one_thunks.insert( ref_thunk_hash );
    }
  }

  /* we don't need to know the thunks that referenced this thunk either */
  referenced_thunks_.erase( old_hash );

  return order_one_thunks;
}

unordered_set<string>
DependencyGraph::order_one_dependencies( const string & thunk_hash ) const
{
  unordered_set<string> result;
  const Thunk & thunk = get_thunk( thunk_hash );

  if ( thunk.executable() ) {
    result.insert( thunk_hash );
    return result;
  }

  for ( const string & hash : thunk.data_thunks() ) {
    unordered_set<string> subresult = order_one_dependencies( hash );
    result.insert( subresult.begin(), subresult.end() );
  }

  return result;
}

string DependencyGraph::updated_hash( const string & original_hash ) const
{
  if ( updated_hashes_.count( original_hash ) ) {
    return updated_hashes_.at( original_hash );
  }
  else {
    return original_hash;
  }
}

string DependencyGraph::original_hash( const string & updated_hash ) const
{
  if ( original_hashes_.count( updated_hash ) ) {
    return original_hashes_.at( updated_hash );
  }
  else {
    return updated_hash;
  }
}
