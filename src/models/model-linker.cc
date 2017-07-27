/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "model-gcc.hh"

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <array>
#include <regex>
#include <unordered_set>

using namespace std;

vector<string> get_link_dependencies( const vector<InputFile> & link_inputs,
                                      const vector<string> & gcc_args )
{
  unordered_set<string> dependencies;
  vector<string> args { gcc_args };

  size_t last_index = SIZE_MAX;
  for ( auto it = link_inputs.rbegin(); it != link_inputs.rend(); it++ ) {
    if ( it->language == Language::SHARED_LIBRARY ) {
      continue;
    }

    assert( it->index < last_index );
    args.erase( args.begin() + it->index );
    last_index = it->index;
  }

  args.insert( args.begin(), ( toolchain_path / GCC ).string() );
  args.push_back( "-Wl,--verbose" );
  args.push_back( "-B" + gcc_install_path );

  ostringstream command;
  copy( args.begin(), args.end(), ostream_iterator<string>( command, " " ) );
  command << "2>&1";

  std::shared_ptr<FILE> readpipe( popen( command.str().c_str(), "r" ), pclose );

  array<char, 4096> buffer;
  size_t seperator_line_count = 0;

  regex path_regex( "^(?:attempt to open|opened script file|found [^\\s]+ at) "
                    "([^\\s]+)\\s?(?:succeeded|)\n$" );
  smatch match;

  while ( !feof( readpipe.get() ) ) {
    if ( fgets( buffer.data(), 4096, readpipe.get() ) != nullptr ) {
      string line = buffer.data();

      if ( seperator_line_count < 2 and line[ 0 ] == '=' ) {
        seperator_line_count++;
        continue;
      }
      else if ( seperator_line_count < 2 ) {
        continue;
      }

      regex_match( line, match, path_regex );
      if ( match.size() == 2 ) {
        dependencies.insert( match[ 1 ].str() );
      }
    }
  }

  return { dependencies.begin(), dependencies.end() };
}
