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

#include "thunk/thunk.hh"
#include "util/optional.hh"

class ExecutionGraph
{
private:
  std::unordered_map<std::string, gg::thunk::Thunk> thunks_ {};

  std::unordered_map<std::string, std::unordered_set<std::string>> referencing_thunks_ {};

  std::unordered_set<std::string> value_dependencies_ {};
  std::unordered_set<std::string> executable_dependencies_ {};

  std::unordered_map<std::string, std::string> original_hashes_ {};
  std::unordered_map<std::string, std::string> updated_hashes_ {};

  void update_hash( const std::string & old_hash,
                    const std::vector<gg::ThunkOutput> & outputs );

public:
  std::string add_thunk( const std::string & hash );

  Optional<std::unordered_set<std::string>>
  force_thunk( const std::string & old_hash,
               std::vector<gg::ThunkOutput> && outputs );

  const std::unordered_set<std::string> &
  value_dependencies() const { return value_dependencies_; }

  const std::unordered_set<std::string> &
  executable_dependencies() const { return executable_dependencies_; }

  std::unordered_set<std::string>
  order_one_dependencies( const std::string & hash ) const;

  const gg::thunk::Thunk &
  get_thunk( const std::string & hash ) const { return thunks_.at( hash ); }

  std::string updated_hash( const std::string & original_hash ) const;
  std::string original_hash( const std::string & updated_hash ) const;
  size_t size() const { return thunks_.size(); }
};

#endif /* GRAPH_HH */
