/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef EXECUTION_RESPONSE_HH
#define EXECUTION_RESPONSE_HH

#include <string>
#include <exception>
#include <stdexcept>
#include <sys/types.h>

#include "optional.hh"

class FetchDependenciesError : public std::exception {};
class ExecutionError : public std::exception {};
class UploadOutputError : public std::exception {};

enum class JobStatus
{
  Success = 0,
  RateLimit,
  InvocationFailure,
  OperationalFailure,
  FetchDependenciesFailure,
  ExecutionFailure,
  UploadOutputFailure,
  SocketFailure,
  ChildProcessFailure
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
  std::string output;

  static ExecutionResponse parse_message( const std::string & message );
};

#endif /* REMOTE_RESPONSE_HH */
