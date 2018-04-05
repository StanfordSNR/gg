/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef ENGINE_MEOW_HH
#define ENGINE_MEOW_HH

#include <vector>

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
    enum class State { UNINITIALIZED, FREE, BUSY };

    State state { State::UNINITIALIZED };
  };

  AWSCredentials credentials_;
  std::string region_;
  Address aws_addr_;
  Address listen_addr_;
  TCPSocket listen_socket_;
  SSLContext ssl_context_ {};

  std::map<uint64_t, Lambda> lambdas_ {};
  std::set<uint64_t> free_lambdas_ {};

  HTTPRequest generate_request();

public:
  MeowExecutionEngine( const AWSCredentials & credentials,
                       const std::string & region,
                       const Address & listen_addr );

  void init( ExecutionLoop & loop ) override;

  void force_thunk( const gg::thunk::Thunk & thunk,
                    ExecutionLoop & exec_loop ) override;

  bool is_remote() const { return true; }
  bool can_execute( const gg::thunk::Thunk & thunk ) const override;
  size_t job_count() const override;
  std::string label() const override { return "meow"; }
};

#endif /* ENGINE_MU_HH */
