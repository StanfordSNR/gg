/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef SANDBOX_HH
#define SANDBOX_HH

#include <string>
#include <functional>
#include <unordered_map>
#include <unordered_set>

#include "traced_process.hh"

struct Permissions
{
  bool read, write, execute;
};

enum LogLevel
{
  LOG_LEVEL_NO_LOG = 0,
  LOG_LEVEL_DEBUG,
};

class SandboxedProcess
{
private:
  TracedProcess process_;
  LogLevel log_level_ { LOG_LEVEL_NO_LOG };

  /* map from pathname to access modes (O_RDONLY, O_WRONLY, O_RDRW) */
  std::unordered_map<std::string, Permissions> allowed_files_;
  std::unordered_set<std::string> allow_candidates_ {};

  bool file_syscall_entry( const SystemCallInvocation & syscall );

  bool open_entry( const SystemCallInvocation & syscall );
  bool open_exit( const SystemCallInvocation & syscall );
  bool rename_entry( const SystemCallInvocation & syscall )

public:
  SandboxedProcess( std::function<int()> && child_procedure,
                    const std::unordered_map<std::string, Permissions> & allowed_files );

  /* throws an exception if sandbox violation happens. */
  void execute();

  void set_log_level( const LogLevel log_level ) { log_level_ = log_level; }

  Optional<int> exit_status() { return process_.exit_status(); }
};

class SandboxViolation : public std::runtime_error
{
public:
  SandboxViolation( const std::string & s_error, const std::string & details )
    : runtime_error( s_error + ": " + details )
  {}
};


#endif /* SANDBOX_HH */
