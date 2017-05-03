/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef SANDBOX_HH
#define SANDBOX_HH

#include <string>
#include <functional>
#include <unordered_set>

#include "traced_process.hh"

class Sandbox
{
private:
  TracedProcess process_;
  std::unordered_set<std::string> allowed_reads_;
  std::unordered_set<std::string> allowed_writes_;

public:
  Sandbox( std::function<int()> && child_procedure,
           const std::unordered_set<std::string> & allowed_reads,
           const std::unordered_set<std::string> & allowed_writes );

  /* throws an exception if sandbox violation happens. */
  void execute();
};

class SandboxViolation : public std::runtime_error
{
public:
  SandboxViolation( const std::string & s_error )
    : runtime_error( s_error )
  {}
};


#endif /* SANDBOX_HH */
