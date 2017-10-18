/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "timeit.hh"

#include <chrono>

using namespace std;

template<class TimeUnit>
TimeUnit time_it( const function<void()> & f )
{
  chrono::high_resolution_clock::time_point begin = chrono::high_resolution_clock::now();
  f();
  chrono::high_resolution_clock::time_point end = chrono::high_resolution_clock::now();
  return chrono::duration_cast<TimeUnit>( end - begin );
}

template chrono::milliseconds time_it( const function<void()> & f );
