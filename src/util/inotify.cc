#include "inotify.hh"

#include <sys/inotify.h>
#include <limits.h>
#include <unistd.h>
#include <stdexcept>

#include "exception.hh"

using namespace std;
using namespace PollerShortNames;

Inotify::Inotify(Poller & poller)
  : inotify_fd_(CheckSystemCall("inotify_init", inotify_init())),
    map_()
{
  poller.add_action(
    Poller::Action(inotify_fd_, Direction::In,
      [this]() {
        return handle_events();
      }
    )
  );
}

int Inotify::add_watch(const roost::path & path,
                        const uint32_t mask,
                        const callback_t & callback)
{
  int wd = CheckSystemCall(
             "inotify_add_watch",
             inotify_add_watch(inotify_fd_.fd_num(), path.string().c_str(), mask));

  /* insert a new key-value pair or update the current value */
  map_[wd] = make_tuple(path, mask, callback);

  return wd;
}

vector<int> Inotify::add_watch(const vector<roost::path> & paths,
                                const uint32_t mask,
                                const callback_t & callback)
{
  vector<int> wd_list;
  for (const auto & path : paths) {
    wd_list.emplace_back(add_watch(path, mask, callback));
  }

  return wd_list;
}

void Inotify::rm_watch(const int wd)
{
  CheckSystemCall("inotify_rm_watch",
                  inotify_rm_watch(inotify_fd_.fd_num(), wd));

  auto erase_ret = map_.erase(wd);
  if (erase_ret == 0) {
    throw runtime_error(
      "rm_watch: trying to remove a nonexistent watch descriptor");
  }
}

Result Inotify::handle_events()
{
  /* explicitly ensure the buffer is sufficient to read at least one event */
  const int BUF_LEN = sizeof(inotify_event) + NAME_MAX + 1;

  /* read events */
  string event_buf = inotify_fd_.read(BUF_LEN);

  const char * buf = event_buf.c_str();
  const inotify_event * event;

  /* loop over all events in the buffer */
  for (const char * ptr = buf; ptr < buf + event_buf.size(); ) {
    event = reinterpret_cast<const inotify_event *>(ptr);

    auto map_it = map_.find(event->wd);
    /* ignore events from an unwatched descriptor */
    if (map_it != map_.end()) {
      const auto & path = get<0>( map_it->second );
      const auto & mask = get<1>( map_it->second );
      const auto & callback = get<2>( map_it->second );

      /* ignore events not interested in */
      if ((event->mask & mask) != 0) {
        callback(*event, path);
      }
    }

    ptr += sizeof(inotify_event) + event->len;
  }

  return ResultType::Continue;
}
