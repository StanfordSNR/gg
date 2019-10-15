/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=8 sts=2 sw=2 tw=80 : */

#ifndef GRAPH_HH
#define GRAPH_HH

#include <string>
#include <memory>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <mutex>

#include "thunk/thunk.hh"
#include "util/optional.hh"

typedef std::string Hash;

// A computation denotes a node in the graph which is in the process of being reduced.
// If (+ 3 4) was reduced to 7, then both would have the same ComputationId,
// just at different points in time.
typedef size_t ComputationId;
struct Computation {
  // Whether the `thunk` field accurately reflects the current state
  bool up_to_date {false};
  // The executable (but potentially out-of-date) thunk
  gg::thunk::Thunk thunk;

  // If we detemine that this computation is equivalent to another one, then
  // this field is set to true, and the deps will be that node.
  bool is_link_{false};

  // The outputs for this computation. Empty if a thunk. Non-empty if a value.
  std::vector<gg::ThunkOutput> outputs {};

  // Computations that this one depends on.
  std::unordered_set<ComputationId> deps {};
  // Computations dependent on this one.
  std::unordered_set<ComputationId> rev_deps {};
  // The hashes that this thunk knows its dependencies by
  std::unordered_map<ComputationId, Hash> dep_hashes {};

  Computation( gg::thunk::Thunk && thunk );

  bool is_value() const { return not outputs.empty(); }
  bool is_link() const { return is_link_; }
  bool is_thunk() const { return outputs.empty() and not is_link_; }

  // Return whether a reduction from `hash` is applicable to this node.
  bool is_reducible_from_hash( const std::string& hash ) const;
};

class ExecutionGraph
{
private:
  // Computations, maybe irreducible, maybe not
  std::unordered_map<ComputationId, Computation> computations_ {};

  // Number of computations that must be reduced
  size_t n_unreduced {0};

  // The ComputationId for known hashes
  std::unordered_map<Hash, ComputationId> ids_ {};

  // The hashes of pre-existing blobs (values or executables) that this
  // function depends on.
  std::unordered_set<Hash> blob_dependencies_ {};

  ComputationId next_id_ {0};

  // Place the thunk at the indicated location, and pull in dependencies
  void _emplace_thunk( ComputationId id,
                       gg::thunk::Thunk && thunk );

  // Given a computation `id`, ensures that said `id` either:
  //    * refers to a value OR
  //    * refers to an up-to-date computation (one with a thunk that reflects
  //    the current comptution)
  void _update( const ComputationId id );

  // Given a computation `id`, marks this `id` (and all transitive dependents)
  // as out-of-date.
  void _mark_out_of_date( const ComputationId id );

  // Record that `from` depends on `on`. `on_hash` is the **parent's** name for
  // the child.
  void _create_dependency( const ComputationId from,
                           const Hash & on_hash,
                           const ComputationId on );

  // Remove the dependency from `from` on `on`.
  void _erase_dependency( const ComputationId from,
                          const ComputationId on );

  // Cut the dependencies from `computation`
  void _cut_dependencies( const ComputationId id );

  // Get a list of executable thunks
  std::unordered_set<Hash>
  order_one_dependencies( const ComputationId id ) const;

  // Given `id`, follow link pointer until a computation that does not have any
  // links.
  ComputationId _follow_links( const ComputationId id ) const;

  // Given an `id`, returns all ancestor computations that have one thunk on the
  // path from them to this `id`.
  std::unordered_set<ComputationId>
  _one_thunk_ancestors( const ComputationId id ) const;

public:

  // Adds a computation for this thunk to the graph. A No-op if present.
  // If `trace` is set, then this thunk will be queryable
  ComputationId add_thunk( const Hash & hash );

  // Given a `hash`, determines the value of that hash, if it is known.
  Optional<Hash> query_value( const Hash & hash ) const;

  // Informs that graph that `from` reduces to `to`.
  // Returns newly executable thunks.
  std::unordered_set<Hash>
  submit_reduction( const Hash & from, std::vector<gg::ThunkOutput> && to );

  // Get a list of executable thunks
  std::unordered_set<Hash>
  order_one_dependencies( const Hash & hash ) const;

  // Get initial blobs that the computation is dependent on
  const std::unordered_set<Hash> &
  blob_dependencies() const { return blob_dependencies_; }

  size_t size() const { return n_unreduced; }
};

#endif /* GRAPH_HH */
