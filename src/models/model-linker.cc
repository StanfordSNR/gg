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
using namespace gg::thunk;

vector<string> GCCModelGenerator::get_link_dependencies( const vector<InputFile> & link_inputs,
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


Thunk GCCModelGenerator::generate_link_thunk( const vector<InputFile> & link_inputs,
                                              const vector<string> & link_args,
                                              const vector<string> & dependencies,
                                              const string & output )
{
  vector<InFile> infiles;
  infiles.emplace_back( program_infiles.at( GCC ) );
  infiles.emplace_back( program_infiles.at( COLLECT2 ) );
  infiles.emplace_back( program_infiles.at( LD ) );

  for ( auto const & link_input : link_inputs ) {
    if ( link_input.source_language == Language::OBJECT or
         link_input.source_language == Language::ARCHIVE_LIBRARY ) {
      infiles.emplace_back( link_input.name );
    }
  }

  for ( const string & dep : dependencies ) {
    infiles.emplace_back( dep );
  }

  vector<string> args;
  args.reserve( link_args.size() + gcc_library_path.size() );

  for ( const string & dir : gcc_library_path ) {
    infiles.emplace_back( dir, InFile::Type::DUMMY_DIRECTORY );
    args.push_back( "-L" + dir );
  }

  for ( const string & dir : ld_search_path ) {
    infiles.emplace_back( dir, InFile::Type::DUMMY_DIRECTORY );
  }

  infiles.emplace_back( gcc_install_path, InFile::Type::DUMMY_DIRECTORY );

  args.insert( args.end(), link_args.begin(), link_args.end() );

  args.push_back( "-B" + gcc_install_path );

  return { output, gcc_function( args, envars_ ), infiles };
}
