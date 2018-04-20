/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef REDUCTOR_HH
#define REDUCTOR_HH

#include <string>
#include <vector>
#include <deque>
#include <memory>
#include <unordered_set>

#include "loop.hh"
#include "engine.hh"
#include "thunk/graph.hh"
#include "storage/backend.hh"

class Reductor
{
private:
  const std::vector<std::string> target_hashes_;
  std::unordered_set<std::string> remaining_targets_;
  size_t max_jobs_;
  bool status_bar_;

  ExecutionGraph dep_graph_ {};

  std::deque<std::string> job_queue_ {};
  std::unordered_set<std::string> running_jobs_ {};
  size_t finished_jobs_ { 0 };
  float estimated_cost_ { 0.0 };

  int base_poller_timeout_ { -1 };
  int poller_timeout_ { -1 };

  ExecutionLoop exec_loop_ {};
  std::vector<std::unique_ptr<ExecutionEngine>> exec_engines_;

  std::unique_ptr<StorageBackend> storage_backend_;

  void finalize_execution( const std::string & old_hash,
                           std::vector<gg::ThunkOutput> && outputs,
                           const float cost = 0.0 );

  size_t running_jobs() const;
  bool is_finished() const;

public:
  Reductor( const std::vector<std::string> & target_hashes,
            const size_t max_jobs,
            std::vector<std::unique_ptr<ExecutionEngine>> && execution_engines,
            std::unique_ptr<StorageBackend> && storage_backend,
            const int base_timeout = -1,
            const bool status_bar = false );

  std::vector<std::string> reduce();
  void upload_dependencies() const;
  void download_targets( const std::vector<std::string> & hashes ) const;
  void print_status() const;
};

#endif /* REDUCTOR_HH */
