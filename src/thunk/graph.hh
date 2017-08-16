/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef GRAPH_HH
#define GRAPH_HH

#include <string>
#include <memory>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <mutex>

#include "thunk.hh"

class DependencyGraph
{
private:
  std::mutex unique_id_mutex_ {};

  std::unordered_map<std::string, size_t> hash_to_id_ {};
  std::map<size_t, std::string> id_to_hash_ {};

  std::map<size_t, gg::thunk::Thunk> thunks_ {};
  std::map<size_t, std::set<size_t>> referenced_thunks_ {};

  size_t insert_thunk_hash( const std::string & hash );

public:
  DependencyGraph();

  size_t add_thunk( const std::string & hash );
};

#endif /* GRAPH_HH */
