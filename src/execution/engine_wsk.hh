/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef ENGINE_WSK_HH
#define ENGINE_WSK_HH

#include <unordered_map>
#include <chrono>

#include "engine.hh"
#include "aws.hh"
#include "lambda.hh"
#include "thunk.hh"
#include "http_request.hh"
#include "optional.hh"

class OpenWhiskExecutionEngine : public ExecutionEngine
{
private:
  std::string endpoint_;
  std::string auth_;

  std::string hostname_;
  std::string path_;

  Optional<std::ofstream> fout_timelog_ { false };

  Address address_;
  SSLContext ssl_context_ {};

  size_t running_jobs_ { 0 };
  std::unordered_map<std::string, std::chrono::steady_clock::time_point> start_times_ {};

  HTTPRequest generate_request( const gg::thunk::Thunk & thunk,
                                const std::string & thunk_hash );

  static float compute_cost( const std::chrono::steady_clock::time_point &,
                             const std::chrono::steady_clock::time_point & = std::chrono::steady_clock::now() ) { return 0; }

  static std::pair<std::string, std::string> get_host_and_path( const std::string & url );

public:
  OpenWhiskExecutionEngine( const std::string & endpoint,
                            const std::string & auth,
                            const std::string & timelog,
                            ExecutionLoop & loop, CallbackFunc callback )
    : ExecutionEngine( loop, callback ), endpoint_( endpoint ),
      auth_( auth ), hostname_( get_host_and_path( endpoint_ ).first ),
      path_( get_host_and_path( endpoint_ ).second ),
      address_( hostname_, "https" )
  {
    if ( timelog.length() > 0 ) {
      fout_timelog_.initialize( timelog ); 
    }
  }

  void force_thunk( const std::string & hash, const gg::thunk::Thunk & thunk ) override;
  size_t job_count() const override;

  bool is_remote() const { return true; }
  std::string label() const override { return "wsk"; }
  bool can_execute( const gg::thunk::Thunk & thunk ) const override;
};

#endif /* ENGINE_WSK_HH */
