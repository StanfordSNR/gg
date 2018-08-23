/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef UTIL_URI_HH
#define UTIL_URI_HH

#include <string>
#include <unordered_map>

#include "optional.hh"

struct ParsedURI
{
  std::string protocol {};
  std::string username {};
  std::string password {};
  std::string host {};
  Optional<uint16_t> port { 0 };
  std::string path {};
  std::unordered_map<std::string, std::string> options {};

  ParsedURI( const std::string & uri );
};

#endif /* UTIL_URI_HH */
