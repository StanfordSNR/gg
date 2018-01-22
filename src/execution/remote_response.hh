/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef REMOTE_RESPONSE_HH
#define REMOTE_RESPONSE_HH

#include <string>
#include <sys/types.h>

enum class JobStatus
{
  Success,
  RateLimit,
  InvocationFailure,
  ExecutionFailure
};

class RemoteResponse
{
private:
  RemoteResponse();

public:
  JobStatus status;
  std::string thunk_hash;
  std::string output_hash;
  off_t output_size;
  bool is_executable;

  static RemoteResponse parse_message( const std::string & message );
};

#endif /* REMOTE_RESPONSE_HH */
