/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=8 sts=2 sw=2 tw=80 : */


#include "graph.hh"

#include <stdexcept>

#include "ggutils.hh"
#include "thunk.hh"
#include "thunk_reader.hh"
#include "thunk_writer.hh"

using namespace std;
using namespace gg;
using namespace gg::thunk;

Computation::Computation( Thunk && thunk )
  : thunk( move( thunk ) ) {}

bool Computation::is_reducible_from_hash( const string & hash ) const
{
  return is_thunk() and up_to_date and thunk.hash() == hash;
}

ComputationId ExecutionGraph::add_thunk( const Hash & full_hash )
{
  // Strip output name
  const string hash = gg::hash::base( full_hash );

  // If hash is present, we're done
  if ( ids_.count( hash ) ) {
    return ids_.at( hash );
  }

  // Get thunk
  Thunk thunk { move( ThunkReader::read( gg::paths::blob( hash ), hash ) ) };

  // Add blob dependencies
  for ( const Thunk::DataItem & item : thunk.values() ) {
    blob_dependencies_.emplace( item.first );
  }
  for ( const Thunk::DataItem & item : thunk.executables() ) {
    blob_dependencies_.emplace( item.first );
  }

  // Create node in the graph
  ComputationId id = next_id_++;
  _emplace_thunk( id, move( thunk ) );
  return id;
}

void ExecutionGraph::_emplace_thunk( ComputationId id,
                                     gg::thunk::Thunk && thunk )
{
  // First, put the computation into the graph
  if ( computations_.count( id ) ) {
    Computation & computation = computations_.at( id );
    computation.thunk = move( thunk );
  } else {
    n_unreduced++;
    computations_.emplace( id, Computation { move( thunk ) } );
  }

  // Now, check the hash of the computation
  Computation & computation = computations_.at( id );
  const Hash hash = computation.thunk.hash();
  if ( ids_.count( hash ) ) {
    // If we already have a node with this hash, link our node to it.
    const ComputationId target_id = ids_.at( hash );
    computation.is_link_ = true;
    _cut_dependencies( id );
    _update( target_id );
    _create_dependency( id, hash, target_id );
  } else {
    // If we do not, record the hash
    ids_.insert( { move( hash ), id } );
    // and add dependencies
    for ( const Thunk::DataItem & item : computation.thunk.thunks() ) {
      const string child_hash = gg::hash::base( item.first );
      ComputationId child_id = add_thunk( child_hash );
      _create_dependency( id, child_hash, child_id );
    }
  }

  // Update our thunk
  _update( id );
}

void ExecutionGraph::_mark_out_of_date( const ComputationId id )
{
  Computation & computation = computations_.at( id );
  if ( not computation.is_value() and computation.up_to_date ) {
    computation.up_to_date = false;
    for ( const ComputationId parent_id : computation.rev_deps ) {
      _mark_out_of_date( parent_id );
    }
  }
}

void ExecutionGraph::_update( const ComputationId id )
{
  Computation & computation = computations_.at( id );
  // Our computation could be an (out of date) link
  if ( computation.is_link() and not computation.up_to_date ) {
    // Check the (single) child for valuation
    ComputationId child_id = *computation.deps.begin();
    Computation & child = computations_.at( child_id );
    _update( child_id );
    if ( child.is_value() ) {
      computation.is_link_ = false;
      computation.outputs = child.outputs;
      n_unreduced--;
      _erase_dependency( id, child_id );
    }
  // Or it could be an (out of date) thunk
  } else if ( computation.is_thunk() and not computation.up_to_date ) {
    // update all dependencies
    std::unordered_set<ComputationId> deps{computation.deps};
    for ( const ComputationId child_id : deps ) {
      _update( child_id );
      Computation & child = computations_.at( child_id );
      string old_hash = computation.dep_hashes.at( child_id );
      if ( child.is_value() ) {
        computation.thunk.update_data( old_hash, child.outputs );
        _erase_dependency( id, child_id );
      } else {
        string new_hash = child.thunk.hash();
        computation.thunk.update_data( old_hash, { { new_hash, "" } } );
        computation.dep_hashes[ child_id ] = new_hash;
      }
    }
  }
  computation.up_to_date = true;
}

void ExecutionGraph::_create_dependency( const ComputationId from,
                                         const Hash & on_hash,
                                         const ComputationId on )
{
  Computation & parent = computations_.at( from );
  Computation & child = computations_.at( on );
  if ( not child.up_to_date ) {
    throw runtime_error( "cannot depend on out-of-date computations" );
  }
  if ( parent.dep_hashes.count( on ) > 0 and parent.dep_hashes.at( on ) != on_hash ) {
    throw runtime_error(
        to_string( from ) + " depended on computation #" + to_string( on ) +
        " for hash " + parent.dep_hashes.at( on ) +
        " but now it should depend on it for hash " + on_hash );
  }

  parent.dep_hashes[ on ] = on_hash;
  parent.deps.insert( on );
  child.rev_deps.insert( from );
}

void ExecutionGraph::_erase_dependency( const ComputationId from,
                                        const ComputationId on ) {
  Computation & parent = computations_.at( from );
  Computation & child = computations_.at( on );
  parent.deps.erase( on );
  parent.dep_hashes.erase( on );
  child.rev_deps.erase( from );
}

void ExecutionGraph::_cut_dependencies( const ComputationId id )
{
  Computation & computation = computations_.at( id );
  for ( const ComputationId child : computation.deps ) {
    computations_.at( child ).rev_deps.erase( id );
  }
  computation.deps.clear();
  computation.dep_hashes.clear();
}

ComputationId ExecutionGraph::_follow_links( ComputationId id ) const
{
  while (computations_.at( id ).is_link()) {
    id = *computations_.at( id ).deps.begin();
  }
  return id;
}

std::unordered_set<ComputationId>
ExecutionGraph::_one_thunk_ancestors( const ComputationId id ) const
{
  const Computation & computation = computations_.at( id );
  if ( computation.is_thunk() ) {
    return { id };
  } else {
    std::unordered_set<ComputationId> parents;
    for ( const ComputationId parent_id : computation.rev_deps ) {
      auto subresult = _one_thunk_ancestors( parent_id );
      parents.insert( subresult.begin(), subresult.end() );
    }
    return parents;
  }
}

Optional<Hash> ExecutionGraph::query_value( const Hash & hash ) const {
  if ( ids_.count( hash ) == 0 ) {
    return {};
  }
  const Computation & computation = computations_.at( ids_.at( hash ) );
  if ( computation.is_value() ) {
    return { true, computation.outputs.front().hash };
  } else {
    return {};
  }
}

std::unordered_set<Hash>
ExecutionGraph::submit_reduction( const Hash & from,
                                  std::vector<gg::ThunkOutput> && to )
{
  // Assert non-empty output
  vector<ThunkOutput> outputs { move( to ) };
  if ( outputs.size() == 0 ) {
    throw runtime_error( "ExecutionGraph::submit_reduction: empty `to`" );
  }

  // If this is a self reduction, return "no new work"
  if ( from == outputs.front().hash ) {
    return {};
  }

  // Assert non-value initial hash
  if ( gg::hash::type( from ) == gg::ObjectType::Value ) {
    throw runtime_error(
        "ExecutionGraph::submit_reduction: cannot reduce value: " + from );
  }

  // If this hash is unknown, ignore it
  if ( ids_.count( from ) == 0 ) {
    return {};
  }

  // If this reduction is for an out-of-date thunk, ignore it
  ComputationId id = ids_.at( from );
  Computation & computation = computations_.at( id );
  if ( not computation.is_reducible_from_hash( from ) ) {
    return {};
  }

  // Finally, accept the reduction
  _mark_out_of_date( id );
  _cut_dependencies( id );

  string & new_hash = outputs.front().hash;
  const gg::ObjectType new_type = gg::hash::type( new_hash );

  if ( new_type == gg::ObjectType::Thunk ) {
    // A new thunk -- return leaf dependencies
    Thunk thunk { move( ThunkReader::read( gg::paths::blob( new_hash ), new_hash ) ) };
    // TODO check that value & executable deps are present
    _emplace_thunk( id, move( thunk ) );
    return order_one_dependencies( id );
  } else {
    // A value -- return reverse dependencies
    computation.outputs = move( outputs );
    n_unreduced--;
    std::unordered_set<Hash> next_to_execute;
    unordered_set<ComputationId> maybe_ready = _one_thunk_ancestors( id );
    for ( const ComputationId parent_id : maybe_ready ) {
      Computation & parent = computations_.at( parent_id );
      _update( parent_id );
      if ( parent.thunk.can_be_executed() ) {
        // Make sure to record the hash and write the thunk before submitting
        ids_[ parent.thunk.hash() ] = parent_id;
        ThunkWriter::write( parent.thunk );
        next_to_execute.insert( parent.thunk.hash() );
      }
    }
    return next_to_execute;
  }
}

std::unordered_set<Hash>
ExecutionGraph::order_one_dependencies( const Hash & input_hash ) const
{
  const string hash = gg::hash::base( input_hash );
  if ( ids_.count( hash ) == 0 ) {
    throw runtime_error( "thunk hash not found in the execution graph" );
  }
  return order_one_dependencies( ids_.at( hash ) );
}

std::unordered_set<Hash>
ExecutionGraph::order_one_dependencies( const ComputationId id ) const
{
  const Computation & computation = computations_.at( id );

  if ( not computation.up_to_date ) {
    throw runtime_error( "thunk is out-of-date" );
  }

  if ( computation.is_value() ) {
    return {};
  } else if ( computation.is_thunk() and computation.thunk.can_be_executed() ) {
    ThunkWriter::write( computation.thunk );
    return { computation.thunk.hash() };
  } else {
    // TODO replace this O(paths to leaves) alg with an O(nodes) alg.
    std::unordered_set<Hash> result;
    for ( const ComputationId child_id : computation.deps ) {
      auto subresult = order_one_dependencies( child_id );
      result.insert( subresult.begin(), subresult.end() );
    }
    return result;
  }

}
