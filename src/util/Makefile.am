AM_CPPFLAGS = $(CXX14_FLAGS)
AM_CXXFLAGS = $(PICKY_CXXFLAGS) $(EXTRA_CXXFLAGS)

noinst_LIBRARIES = libggutil.a

libggutil_a_SOURCES = exception.hh optional.hh file_descriptor.hh chunk.hh \
                      serialization.hh serialization.cc \
                      child_process.hh child_process.cc \
                      signalmask.hh signalmask.cc \
                      digest.hh digest.cc \
                      system_runner.hh system_runner.cc \
                      temp_file.hh temp_file.cc
