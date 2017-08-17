AM_CPPFLAGS = $(CXX14_FLAGS)
AM_CXXFLAGS = $(PICKY_CXXFLAGS) $(EXTRA_CXXFLAGS)

noinst_LIBRARIES = libggutil.a

libggutil_a_SOURCES = exception.hh optional.hh chunk.hh \
                      file_descriptor.hh file_descriptor.cc  \
                      serialization.hh serialization.cc \
                      child_process.hh child_process.cc \
                      signalmask.hh signalmask.cc \
                      digest.hh digest.cc \
                      system_runner.hh system_runner.cc \
                      temp_file.hh temp_file.cc \
                      temp_dir.hh temp_dir.cc \
                      path.hh path.cc \
                      socketpair.hh socketpair.cc \
                      poller.hh poller.cc \
                      signalfd.hh signalfd.cc \
                      event_loop.hh event_loop.cc \
                      tokenize.hh
