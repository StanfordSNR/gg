/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef UTIL_XDG_HH
#define UTIL_XDG_HH

#include <vector>

#include "path.hh"

namespace xdg {
  constexpr char XDG_DATA_HOME[]   = "XDG_DATA_HOME";
  constexpr char XDG_DATA_DIRS[]   = "XDG_DATA_DIRS";
  constexpr char XDG_CONFIG_HOME[] = "XDG_CONFIG_HOME";
  constexpr char XDG_CONFIG_DIRS[] = "XDG_CONFIG_DIRS";
  constexpr char XDG_CACHE_HOME[]  = "XDG_CACHE_HOME";
  constexpr char XDG_RUNTIME_DIR[] = "XDG_RUNTIME_DIR";

  namespace data {
    roost::path home();
    std::vector<roost::path> dirs();
  }

  namespace config {
    roost::path home();
    std::vector<roost::path> dirs();
  }

  namespace cache {
    roost::path home();
  }

  namespace runtime {
    roost::path dir();
  }

}

#endif /* UTIL_XDG_HH */
