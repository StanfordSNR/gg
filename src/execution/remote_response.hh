/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef REMOTE_RESPONSE_HH
#define REMOTE_RESPONSE_HH

#include <string>
#include <sys/types.h>

#include "optional.hh"

class RemoteResponse
{
private:
  RemoteResponse();

public:
  enum class Type
  {
    SUCCESS,
    EXECUTION_FAILURE,
    LAMBDA_FAILURE,
    RATE_LIMIT
  } type;

  std::string thunk_hash;
  std::string output_hash;
  off_t output_size;
  bool is_executable;
  size_t thunk_exec_time { 0 };

  struct OpenWhiskData {
    size_t duration {};
    std::string fn_name {};
  };

  Optional<OpenWhiskData> wsk_data { false };

  static RemoteResponse parse_message( const std::string & message,
                                       const bool wsk = false );
};

#endif /* REMOTE_RESPONSE_HH */
