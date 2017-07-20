/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "model-gcc.hh"

#include <boost/filesystem.hpp>

using namespace std;

namespace fs = boost::filesystem;

string search_for_library( const string & name )
{
  string expected_filename = "lib" + name + ".so";

  for ( const string & sdir_str : c_library_path ) {
    fs::path search_path { sdir_str };
    fs::path expected_path { search_path / expected_filename };

    if ( fs::exists( expected_path ) ) {
      return expected_path.string();
    }
  }

  return "";
}

vector<string> get_link_dependencies( const vector<InputFile> & link_inputs )
{
  vector<string> dependencies;

  for ( const auto & link_input : link_inputs ) {
    switch ( link_input.language ) {
    case Language::OBJECT:
    case Language::ARCHIVE_LIBRARY:
      dependencies.push_back( link_input.name );
      break;

    case Language::SHARED_LIBRARY:
    {
      string path = search_for_library( link_input.name );

      if ( not path.length() ) {
        throw runtime_error( "could not find shared library: " + link_input.name );
      }

      dependencies.push_back( path );
      break;
    }

    default:
      throw runtime_error( "invalid input for link stage: " + link_input.name );
    }
  }

  return dependencies;
}
