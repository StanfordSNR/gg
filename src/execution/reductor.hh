/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef REDUCTOR_HH
#define REDUCTOR_HH

#include <string>
#include <vector>
#include <deque>
#include <memory>

#include "loop.hh"
#include "engine.hh"
#include "graph.hh"

enum class ExecutionEnvironment { LOCAL, GG_RUNNER, LAMBDA };

class Reductor
{
private:
  const std::vector<std::string> target_hashes_;
  size_t max_jobs_;
  bool status_bar_;

  DependencyGraph dep_graph_ {};

  std::deque<std::string> job_queue_ {};
  size_t finished_jobs_ { 0 };
  float estimated_cost_ { 0.0 };

  ExecutionLoop exec_loop_ {};
  std::vector<std::unique_ptr<ExecutionEngine>> exec_engines_ {};

  void execution_finalize( const std::string & old_hash,
                           const std::string & new_hash,
                           const float cost = 0.0 );

  size_t running_jobs() const;
  bool is_finished() const;

public:
  Reductor( const std::vector<std::string> & target_hashes,
            const size_t max_jobs,
            const std::vector<ExecutionEnvironment> & execution_environments,
            const bool status_bar = false );

  std::vector<std::string> reduce();
  void upload_dependencies() const;
  void print_status() const;
};

#endif /* REDUCTOR_HH */
