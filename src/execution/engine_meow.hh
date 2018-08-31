/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef ENGINE_MEOW_HH
#define ENGINE_MEOW_HH

#include <vector>
#include <memory>
#include <queue>
#include <unordered_set>

#include "engine.hh"
#include "net/address.hh"
#include "net/aws.hh"
#include "net/lambda.hh"
#include "net/socket.hh"

class MeowExecutionEngine : public ExecutionEngine
{
private:
  struct Lambda
  {
    enum class State { Idle, Busy };

    size_t id;
    State state { State::Idle };
    std::shared_ptr<TCPConnection> connection;
    std::unordered_set<std::string> objects {};
    Optional<gg::thunk::Thunk> executing_thunk {};

    Lambda( const size_t id, std::shared_ptr<TCPConnection> && connection )
      : id( id ), connection( std::move( connection ) ) {}
  };

  enum class SelectionStrategy
  {
    First, Random, MostObjects, LargestObject, LRU,
  };

  AWSCredentials credentials_;
  std::string region_;
  Address aws_addr_;
  Address listen_addr_;
  TCPSocket listen_socket_;
  SSLContext ssl_context_ {};

  size_t running_jobs_ { 0 };
  uint64_t current_id_ { 0 };
  std::map<uint64_t, Lambda> lambdas_ {};
  std::set<uint64_t> free_lambdas_ {};

  std::queue<gg::thunk::Thunk> thunks_queue_ {};

  HTTPRequest generate_request();

  uint64_t pick_lambda( const gg::thunk::Thunk & thunk,
                        const SelectionStrategy s = SelectionStrategy::First );

  void prepare_lambda( Lambda & lambda, const gg::thunk::Thunk & thunk );

public:
  MeowExecutionEngine( const size_t max_jobs, const AWSCredentials & credentials,
                       const std::string & region, const Address & listen_addr );

  void init( ExecutionLoop & loop ) override;

  void force_thunk( const gg::thunk::Thunk & thunk,
                    ExecutionLoop & exec_loop ) override;

  bool is_remote() const { return true; }
  bool can_execute( const gg::thunk::Thunk & thunk ) const override;
  size_t job_count() const override;
  std::string label() const override { return "meow"; }
};

#endif /* ENGINE_MU_HH */
