/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef SANDBOX_HH
#define SANDBOX_HH

#include <string>
#include <functional>
#include <unordered_map>

#include "traced_process.hh"

struct Permissions
{
  bool read, write, execute;
};

class SandboxedProcess
{
private:
  TracedProcess process_;

  /* map from pathname to access modes (O_RDONLY, O_WRONLY, O_RDRW) */
  std::unordered_map<std::string, Permissions> allowed_files_;

  bool file_syscall_entry( const SystemCallInvocation & syscall );

  bool open_entry( const SystemCallInvocation & syscall );

public:
  SandboxedProcess( std::function<int()> && child_procedure,
                    const std::unordered_map<std::string, Permissions> & allowed_files );

  /* throws an exception if sandbox violation happens. */
  void execute();
};

class SandboxViolation : public std::runtime_error
{
public:
  SandboxViolation( const std::string & s_error, const std::string & details )
    : runtime_error( s_error + ": " + details )
  {}
};


#endif /* SANDBOX_HH */
