/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef UTIL_HH
#define UTIL_HH

#include <string>

std::string safe_getenv( const std::string & key );

std::string safe_getenv_or( const std::string & key, const std::string & def_val );

#endif /* UTIL_HH */
