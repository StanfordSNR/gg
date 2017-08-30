/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef PIPE_HH
#define PIPE_HH

#include <utility>

#include "file_descriptor.hh"

std::pair<FileDescriptor, FileDescriptor> make_pipe();

#endif /* PIPE_HH */
