/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "graph.hh"

#include <stdexcept>

#include "ggutils.hh"
#include "thunk.hh"
#include "thunk_reader.hh"
#include "thunk_writer.hh"

using namespace std;
using namespace gg;
using namespace gg::thunk;

string ExecutionGraph::add_thunk( const string & full_hash )
{
  const string hash = gg::hash::base( full_hash );
  const string & updated = updated_hash( hash );

  if ( thunks_.count( updated ) ) {
    return updated;
  }

  if ( thunks_.count( hash ) ) {
    return hash;
  }

  Thunk thunk { move( ThunkReader::read( gg::paths::blob( hash ), hash ) ) };

  /* creating the entry */
  referencing_thunks_[ hash ];

  for ( const Thunk::DataItem & item : thunk.values() ) {
    value_dependencies_.emplace( item.first );
  }

  for ( const Thunk::DataItem & item : thunk.executables() ) {
    executable_dependencies_.emplace( item.first );
  }

  vector<pair<string, string>> updates_to_thunk;

  for ( const Thunk::DataItem & item : thunk.thunks() ) {
    const string item_base = gg::hash::base( item.first );
    const string item_updated = add_thunk( item_base );
    referencing_thunks_[ item_updated ].emplace( hash );

    if ( item_updated != item_base ) {
      updates_to_thunk.emplace_back( item_base, item_updated );
    }
  }

  for ( const pair<string, string> & update : updates_to_thunk ) {
    vector<ThunkOutput> new_outputs;
    for ( const auto & output : thunk.outputs() ) {
      new_outputs.emplace_back( update.second, output );
    }

    thunk.update_data( update.first, new_outputs );
  }

  thunks_.emplace( piecewise_construct,
                   forward_as_tuple( hash ),
                   forward_as_tuple( move( thunk ) ) );

  return hash;
}

void ExecutionGraph::update_hash( const string & old_hash,
                                  const vector<ThunkOutput> & outputs )
{
  const string & new_hash = outputs.front().hash;

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
    referencing_thunk.update_data( old_hash, outputs );
  }

  /* we don't need the old thunk entry */
  thunks_.erase( old_hash );

  /* we don't need the old referencing thunks list */
  referencing_thunks_[ new_hash ] = move( referencing_thunks_.at( old_hash ) );
  referencing_thunks_.erase( old_hash );
}

Optional<unordered_set<string>>
ExecutionGraph::force_thunk( const string & old_hash,
                             vector<ThunkOutput> && original_outputs )
{
  if ( thunks_.count( old_hash ) == 0 ) {
    return { false };
  }

  vector<ThunkOutput> outputs { move( original_outputs ) };

  string & actual_new_hash = outputs.front().hash;

  unordered_set<string> next_to_execute;
  const gg::ObjectType new_type = gg::hash::type( actual_new_hash );

  /* the old thunk has returned a new thunk. this is not a pipe dream. */
  if ( new_type == gg::ObjectType::Thunk ) {
    actual_new_hash = add_thunk( actual_new_hash );
  }

  update_hash( old_hash, outputs );

  for ( const string & referencing_thunk_hash : referencing_thunks_.at( actual_new_hash ) ) {
    Thunk & referencing_thunk = thunks_.at( referencing_thunk_hash );

    if ( referencing_thunk.can_be_executed() ) {
      const string referencing_thunk_new_hash = ThunkWriter::write( referencing_thunk );

      vector<ThunkOutput> new_outputs;
      for ( const auto & output : referencing_thunk.outputs() ) {
        new_outputs.emplace_back( referencing_thunk_new_hash, output );
      }

      thunks_.emplace( piecewise_construct,
                       forward_as_tuple( referencing_thunk_new_hash ),
                       forward_as_tuple( move( referencing_thunk ) ) );

      update_hash( referencing_thunk_hash, new_outputs );
      next_to_execute.emplace( move( referencing_thunk_new_hash ) );
    }
  }

  if ( new_type == gg::ObjectType::Thunk ) {
    next_to_execute = order_one_dependencies( actual_new_hash );
  }
  else {
    /* the thunk has been reducted to a value. we don't need to keep
    the list of thunks that are referencing it anymore. */
    referencing_thunks_.erase( actual_new_hash );
  }

  return { true, move( next_to_execute ) };
}

unordered_set<string> ExecutionGraph::order_one_dependencies( const string & input_hash ) const
{
  const string hash = gg::hash::base( input_hash );

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
