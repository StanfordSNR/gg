/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#pragma once

#include <system_error>
#include <iostream>

class tagged_error : public std::system_error
{
private:
  std::string attempt_and_error_;
  int error_code_;

public:
  tagged_error( const std::error_category & category,
                const std::string s_attempt,
                const int error_code )
    : system_error( error_code, category ),
      attempt_and_error_( s_attempt + ": " + std::system_error::what() ),
      error_code_( error_code )
  {}

  const char * what( void ) const noexcept override
  {
    return attempt_and_error_.c_str();
  }

  int error_code() const { return error_code_; }
};

class unix_error : public tagged_error
{
public:
  unix_error ( const std::string & s_attempt,
               const int s_errno = errno )
    : tagged_error( std::system_category(), s_attempt, s_errno )
  {}
};

inline void print_exception( const char * argv0, const std::exception & e )
{
  std::cerr << argv0 << ": " << e.what() << std::endl;
}

inline int CheckSystemCall( const char * s_attempt, const int return_value )
{
  if ( return_value >= 0 ) {
    return return_value;
  }

  throw unix_error( s_attempt );
}

inline int CheckSystemCall( const std::string & s_attempt, const int return_value )
{
  return CheckSystemCall( s_attempt.c_str(), return_value );
}
