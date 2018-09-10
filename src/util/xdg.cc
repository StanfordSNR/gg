/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "xdg.hh"

#include <string>

#include "util.hh"

using namespace std;

namespace xdg {

  constexpr char HOME[]                    = "HOME";
  constexpr char XDG_DATA_HOME_SUFFIX[]    = "/.local/share";
  constexpr char XDG_CONFIG_HOME_SUFFIX[]  = "/.config";
  constexpr char XDG_CACHE_HOME_SUFFIX[]   = "/.cache";
  constexpr char XDG_DATA_DIRS_DEFAULT[]   = "/usr/local/share/:/usr/share/";
  constexpr char XDG_CONFIG_DIRS_DEFAULT[] = "/etc/xdg";

  namespace cache {

    roost::path get_cache_home()
    {
      roost::path cache_path { safe_getenv_or( XDG_CACHE_HOME, "" ) };

      if ( not roost::is_absolute( cache_path ) ) {
        cache_path = safe_getenv_or( HOME, "" );
        cache_path = cache_path / XDG_CACHE_HOME_SUFFIX;
      }

      if ( not roost::is_absolute( cache_path ) ) {
        cache_path = {};
      }

      return cache_path;
    }

    roost::path home()
    {
      const static roost::path cache_home = get_cache_home();
      return cache_home;
    }

  }

}
