/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef THUNKEXEC_HH
#define THUNKEXEC_HH

#include <exception>
#include <stdexcept>

class FetchDependenciesError : public std::exception {};
class ExecutionError : public std::exception {};
class UploadOutputError : public std::exception {};

enum class ExecuteExitCode
{
  Success = 0,
  FetchDependenciesFailure,
  ExecutionFailure,
  UploadOutputFailure,
  OperationalFailure,
};

#endif /* THUNKEXEC_HH */
