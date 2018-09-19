/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef REDUCTOR_HH
#define REDUCTOR_HH

#include <string>
#include <vector>
#include <deque>
#include <memory>
#include <chrono>
#include <unordered_set>
#include <unordered_map>

#include "loop.hh"
#include "engine.hh"
#include "thunk/graph.hh"
#include "storage/backend.hh"

class Reductor
{
private:
  using Clock = std::chrono::steady_clock;

  struct JobInfo
  {
    Clock::time_point start {};
    std::chrono::milliseconds timeout { 0 };
    uint8_t restarts { std::numeric_limits<uint8_t>::max() };
  };

  const std::vector<std::string> target_hashes_;
  std::unordered_set<std::string> remaining_targets_;
  bool status_bar_;

  ExecutionGraph dep_graph_ {};

  std::deque<std::string> job_queue_ {};
  std::unordered_map<std::string, JobInfo> running_jobs_ {};
  size_t finished_jobs_ { 0 };
  float estimated_cost_ { 0.0 };

  std::chrono::milliseconds default_timeout_;
  size_t timeout_multiplier_;
  std::chrono::milliseconds timeout_check_interval_ { default_timeout_ / 2 };
  Clock::time_point next_timeout_check_ { Clock::now() + timeout_check_interval_ };

  ExecutionLoop exec_loop_ {};
  std::vector<std::unique_ptr<ExecutionEngine>> exec_engines_;
  std::vector<std::unique_ptr<ExecutionEngine>> fallback_engines_;

  std::unique_ptr<StorageBackend> storage_backend_;

  void finalize_execution( const std::string & old_hash,
                           std::vector<gg::ThunkOutput> && outputs,
                           const float cost = 0.0 );

  bool is_finished() const { return ( remaining_targets_.size() == 0 ); }

public:
  Reductor( const std::vector<std::string> & target_hashes,
            std::vector<std::unique_ptr<ExecutionEngine>> && execution_engines,
            std::vector<std::unique_ptr<ExecutionEngine>> && fallback_engines,
            std::unique_ptr<StorageBackend> && storage_backend,
            const std::chrono::milliseconds default_timeout = std::chrono::milliseconds { 0 },
            const size_t timeout_multiplier = 1,
            const bool status_bar = false );

  std::vector<std::string> reduce();
  void upload_dependencies() const;
  void download_targets( const std::vector<std::string> & hashes ) const;
  void print_status() const;
};

#endif /* REDUCTOR_HH */
