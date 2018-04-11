/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef POLLER_HH
#define POLLER_HH

#include <functional>
#include <vector>
#include <cassert>
#include <list>
#include <set>
#include <queue>
#include <poll.h>

#include "file_descriptor.hh"

class NBSecureSocket;

class Poller
{
public:
  struct Action
  {
    struct Result
    {
      enum class Type { Continue, Exit, Cancel, CancelAll } result;
      unsigned int exit_status;
      Result( const Type & s_result = Type::Continue, const unsigned int & s_status = EXIT_SUCCESS )
        : result( s_result ), exit_status( s_status ) {}
    };

    typedef std::function<Result(void)> CallbackType;

    FileDescriptor & fd;
    enum PollDirection : short { In = POLLIN, Out = POLLOUT } direction;
    CallbackType callback;
    std::function<bool(void)> when_interested;
    std::function<void(void)> fderror_callback;
    bool active;

    Action( FileDescriptor & s_fd,
            const PollDirection & s_direction,
            const CallbackType & s_callback,
            const std::function<bool(void)> & s_when_interested = [] () { return true; },
            const std::function<void(void)> & fderror_callback = [] () {} )
      : fd( s_fd ), direction( s_direction ), callback( s_callback ),
        when_interested( s_when_interested ),
        fderror_callback( fderror_callback ), active( true ) {}

    Action( NBSecureSocket & s_socket,
            const PollDirection & s_direction,
            const CallbackType & s_callback,
            const std::function<bool(void)> & s_when_interested = [] () { return true; },
            const std::function<void(void)> & fderror_callback = [] () {} );

    unsigned int service_count( void ) const;
  };

private:
  std::queue<Action> action_add_queue_ {};
  std::list<Action> actions_ {};
  std::vector<pollfd> pollfds_ {};

public:
  struct Result
  {
    enum class Type { Success, Timeout, FDError, Exit } result;
    unsigned int exit_status;
    Result( const Type & s_result, const unsigned int & s_status = EXIT_SUCCESS )
      : result( s_result ), exit_status( s_status ) {}
  };

  Poller() {}

  void add_action( Action action );
  Result poll( const int timeout_ms );

  /* remove all actions for file descriptors in `fd_nums` */
  void remove_actions( const std::set<int> & fd_nums );
};

namespace PollerShortNames {
  typedef Poller::Action::Result Result;
  typedef Poller::Action::Result::Type ResultType;
  typedef Poller::Action::PollDirection Direction;
}

#endif
