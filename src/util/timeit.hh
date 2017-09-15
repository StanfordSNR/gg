/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef TIMEIT_HH
#define TIMEIT_HH

#include <functional>
#include <chrono>

template<class TimeUnit>
TimeUnit time_it( const std::function<void()> & f );

#endif /* TIMEIT_HH */
