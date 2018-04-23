/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef UTIL_HH
#define UTIL_HH

#include <string>

std::string safe_getenv( const std::string & key );
std::string safe_getenv_or( const std::string & key, const std::string & def_val );
std::string format_bytes( size_t bytes );

template <typename E>
constexpr auto to_underlying( E e ) noexcept
{
  return static_cast<std::underlying_type_t<E>>( e );
}

#endif /* UTIL_HH */
