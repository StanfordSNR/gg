/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef EXECUTION_RESPONSE_HH
#define EXECUTION_RESPONSE_HH

#include <string>
#include <sys/types.h>

#include "optional.hh"

enum class JobStatus
{
  Success,
  RateLimit,
  InvocationFailure,
  ExecutionFailure
};

class ExecutionResponse
{
private:
  ExecutionResponse();

public:
  JobStatus status;
  std::string thunk_hash;
  std::string output_hash;
  off_t output_size;
  bool is_executable;

  Optional<std::string> output;

  static ExecutionResponse parse_message( const std::string & message );
};

#endif /* REMOTE_RESPONSE_HH */
