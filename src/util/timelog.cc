/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "util/timelog.hh"

using namespace std;

TimeLog::TimeLog()
  : start_( chrono::system_clock::now() ), prev_( start_ )
{}

void TimeLog::add_point( const std::string & title )
{
  auto now = chrono::system_clock::now();
  points_.emplace_back( title,
                        chrono::system_clock::to_time_t( now ) -
                        chrono::system_clock::to_time_t( prev_ ) );
  prev_ = now;
}
