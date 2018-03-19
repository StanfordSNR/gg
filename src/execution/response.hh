/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef EXECUTION_RESPONSE_HH
#define EXECUTION_RESPONSE_HH

#include <string>
#include <vector>
#include <exception>
#include <stdexcept>
#include <sys/types.h>

#include "util/optional.hh"

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
  ChildProcessFailure,
};

class ExecutionResponse
{
public:
  struct Output
  {
    std::string tag;
    std::string hash;
    off_t size;
    bool is_executable;
    std::string data;
  };

private:
  ExecutionResponse() {}

public:
  JobStatus status {};

  std::string thunk_hash {};
  std::vector<Output> outputs {};

  std::string stdout {};

  static ExecutionResponse parse_message( const std::string & message );
};

#endif /* REMOTE_RESPONSE_HH */
