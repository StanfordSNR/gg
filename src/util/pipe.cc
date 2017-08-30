/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <unistd.h>

#include "pipe.hh"
#include "exception.hh"

using namespace std;

pair<FileDescriptor, FileDescriptor> make_pipe()
{
  int pipe_fds[ 2 ];
  CheckSystemCall( "pipe", pipe( pipe_fds ) );
  return { pipe_fds[ 0 ], pipe_fds[ 1 ] };
}
