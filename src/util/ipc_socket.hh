/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef UTIL_IPC_SOCKET_HH
#define UTIL_IPC_SOCKET_HH

#include <string>

#include "file_descriptor.hh"

/* Unix domain socket */
class IPCSocket : public FileDescriptor
{
public:
  IPCSocket();

  void bind( const std::string & path );
  void connect( const std::string & path );

  void listen( const int backlog = 200 );
  FileDescriptor accept( void );
};

#endif /* UTIL_IPC_SOCKET_HH */
