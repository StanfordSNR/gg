/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef UTIL_TIMELOG_HH
#define UTIL_TIMELOG_HH

#include <vector>
#include <chrono>
#include <string>
#include <ctime>

class TimeLog
{
private:
  std::chrono::milliseconds start_;
  std::chrono::milliseconds prev_;
  std::vector<std::pair<std::string, std::chrono::milliseconds>> points_ {};

public:
  TimeLog();
  void add_point( const std::string & title );

  std::string str() const;
};

#endif /* UTIL_TIMELOG_HH */
