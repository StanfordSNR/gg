#ifndef INOTIFY_HH
#define INOTIFY_HH

#include <sys/inotify.h>
#include <cstdint>
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>
#include <tuple>

#include "file_descriptor.hh"
#include "poller.hh"
#include "path.hh"

/* wrapper class for inotify */
class Inotify
{
public:
  /* callback function type
   * parameter list: inotify event, the path that triggers the event */
  using callback_t = std::function<void(const inotify_event &,
                                        const roost::path &)>;

  Inotify(Poller & poller);

  /* add a single path to the watch list */
  int add_watch(const roost::path & path,
                const uint32_t mask,
                const callback_t & callback);

  /* add multiple paths to the watch list */
  std::vector<int> add_watch(const std::vector<roost::path> & paths,
                             const uint32_t mask,
                             const callback_t & callback);

  /* remove a watch descriptor from the watch list */
  void rm_watch(const int wd);

private:
  /* inotify instance */
  FileDescriptor inotify_fd_;

  /* map a watch descriptor to its associated <path, mask, callback> */
  std::unordered_map<int, std::tuple<roost::path, uint32_t, callback_t>> map_;

  /* handles notified events and tells the poller to continue polling */
  Poller::Action::Result handle_events();
};

#endif /* INOTIFY_HH */
