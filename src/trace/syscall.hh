/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef SYSCALL_HH
#define SYSCALL_HH

#include <cstdlib>
#include <sys/syscall.h>
#include <typeinfo>
#include <typeindex>
#include <string>
#include <vector>
#include <map> /* needed by syscalltbl */

using ArgumentType = std::type_index;

enum ArgumentDirection
{
  ARGUMENT_DIR_IN,
  ARGUMENT_DIR_OUT,
  ARGUMENT_DIR_INOUT,
};

#define DIR_IN ARGUMENT_DIR_IN
#define DIR_OUT ARGUMENT_DIR_OUT
#define DIR_INOUT ARGUMENT_DIR_INOUT

enum ArgumentFlags
{
  ARGUMENT_F_PATHNAME = 0x1,
};

struct ArgumentInfo
{
  ArgumentType type;
  ArgumentDirection dir;
  int flags;
};

class SystemCallSignature
{
private:
  long number_;
  std::string name_;
  std::vector<ArgumentInfo> args_;

public:
  SystemCallSignature( const long number, const std::string & name,
                       const std::vector<ArgumentInfo> & args )
    : number_( number ), name_( name ), args_( args )
  {}

  long number() const { return number_; }
  std::string name() const { return name_; }
  std::vector<ArgumentInfo> args() const { return args_; }
};

#include "syscalltbl.hh"

#endif /* SYSCALL_HH */
