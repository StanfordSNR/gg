/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "graph.hh"

#include <stdexcept>

#include "ggutils.hh"
#include "thunk.hh"
#include "thunk_reader.hh"
#include "thunk_writer.hh"

using namespace std;
using namespace gg::thunk;

void ExecutionGraph::add_thunk( const string & hash )
{
  if ( thunks_.count( hash ) ) {
    /* we already have this thunk */
    return;
  }

  ThunkReader thunk_reader { gg::paths::blob_path( hash ).string() };
  Thunk thunk { thunk_reader.read_thunk() };
  thunk.set_hash( hash );
  referencing_thunks_[ hash ];

  for ( const Thunk::DataItem & item : thunk.values() ) {
    value_dependencies_.emplace( item.first );
  }

  for ( const Thunk::DataItem & item : thunk.executables() ) {
    executable_dependencies_.emplace( item.first );
  }

  for ( const Thunk::DataItem & item : thunk.thunks() ) {
    add_thunk( item.first );
    referencing_thunks_[ item.first ].emplace( hash );
  }

  thunks_.emplace( piecewise_construct,
                   forward_as_tuple( hash ),
                   forward_as_tuple( move( thunk ) ) );
}

void ExecutionGraph::update_hash( const string & old_hash, const string & new_hash )
{
  /* updating the hash chain */
  if ( gg::hash::type( new_hash ) == gg::ObjectType::Thunk ) {
    if ( original_hashes_.count( old_hash ) == 0 ) {
      original_hashes_[ new_hash ] = old_hash;
      updated_hashes_[ old_hash ] = new_hash;
    }
    else {
      original_hashes_[ new_hash ] = original_hashes_[ old_hash ];
      updated_hashes_[ original_hashes_[ old_hash ] ] = new_hash;
      original_hashes_.erase( old_hash );
    }
  }

  /* updating the thunks that are referencing this thunk */
  for ( const string & referencing_thunk_hash : referencing_thunks_.at( old_hash ) ) {
    Thunk & referencing_thunk = thunks_.at( referencing_thunk_hash );
    referencing_thunk.update_data( old_hash, new_hash );
  }

  /* we don't need the old thunk entry */
  thunks_.erase( old_hash );

  /* we don't need the old referencing thunks list */
  referencing_thunks_[ new_hash ] = move( referencing_thunks_.at( old_hash ) );
  referencing_thunks_.erase( old_hash );
}

Optional<unordered_set<string>>
ExecutionGraph::force_thunk( const string & old_hash, const string & new_hash )
{
  if ( thunks_.count( old_hash ) == 0 ) {
    return { false };
  }

  unordered_set<string> next_to_execute;
  const gg::ObjectType new_type = gg::hash::type( new_hash );

  /* the old thunk has returned a new thunk. this is not a pipe dream. */
  if ( new_type == gg::ObjectType::Thunk ) {
    add_thunk( new_hash );
  }

  update_hash( old_hash, new_hash );

  for ( const string & referencing_thunk_hash : referencing_thunks_.at( new_hash ) ) {
    Thunk & referencing_thunk = thunks_.at( referencing_thunk_hash );

    if ( referencing_thunk.can_be_executed() ) {
      string referencing_thunk_new_hash = ThunkWriter::write_thunk( referencing_thunk );
      thunks_.emplace( piecewise_construct,
                       forward_as_tuple( referencing_thunk_new_hash ),
                       forward_as_tuple( move( referencing_thunk ) ) );
      update_hash( referencing_thunk_hash, referencing_thunk_new_hash );
      next_to_execute.emplace( move( referencing_thunk_new_hash ) );
    }
  }

  if ( new_type == gg::ObjectType::Thunk ) {
    next_to_execute = order_one_dependencies( new_hash );
  }
  else {
    /* the thunk has been reducted to a value. we don't need to keep
    the list of thunks that are referencing it anymore. */
    referencing_thunks_.erase( new_hash );
  }

  return { true, move( next_to_execute ) };
}

unordered_set<string> ExecutionGraph::order_one_dependencies( const string & hash ) const
{
  if ( thunks_.count( hash ) == 0 ) {
    throw runtime_error( "thunk hash not found in the execution graph" );
  }

  const Thunk & thunk = thunks_.at( hash );

  if ( thunk.can_be_executed() ) {
    return { hash };
  }

  unordered_set<string> result;

  for ( const Thunk::DataItem & item : thunk.thunks() ) {
    auto subresult = order_one_dependencies( item.first );
    result.insert( subresult.begin(), subresult.end() );
  }

  return result;
}

string ExecutionGraph::updated_hash( const string & original_hash ) const
{
  return updated_hashes_.count( original_hash ) ? updated_hashes_.at( original_hash )
                                                : original_hash;
}

string ExecutionGraph::original_hash( const string & updated_hash ) const
{
  return original_hashes_.count( updated_hash ) ? original_hashes_.at( updated_hash )
                                                : updated_hash;
}
