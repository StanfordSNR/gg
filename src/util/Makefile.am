AM_CPPFLAGS = -I$(srcdir)/. -I$(srcdir)/.. $(CXX14_FLAGS)
AM_CXXFLAGS = $(PICKY_CXXFLAGS) $(EXTRA_CXXFLAGS)

noinst_LIBRARIES = libggutil.a

libggutil_a_SOURCES = exception.hh optional.hh chunk.hh \
                      file_descriptor.hh file_descriptor.cc  \
                      serialization.hh serialization.cc \
                      child_process.hh child_process.cc \
                      digest.hh digest.cc \
                      base64.hh base64.cc \
                      system_runner.hh system_runner.cc \
                      temp_file.hh temp_file.cc \
                      temp_dir.hh temp_dir.cc \
                      util.hh util.cc \
                      path.hh path.cc \
                      pipe.hh pipe.cc \
                      poller.hh poller.cc \
                      signalfd.hh signalfd.cc \
                      tokenize.hh units.hh \
                      timeit.hh timeit.cc \
                      timelog.hh timelog.cc \
                      iterator.hh \
                      uri.hh uri.cc \
                      args.hh args.cc \
                      xdg.hh xdg.cc \
                      inotify.hh inotify.cc \
                      ipc_socket.hh ipc_socket.cc
