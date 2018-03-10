/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "model-gcc.hh"

#include <vector>
#include <string>
#include <sstream>
#include <regex>
#include <iostream>
#include <algorithm>

#include "thunk/ggutils.hh"
#include "thunk/thunk_reader.hh"
#include "util/digest.hh"
#include "util/system_runner.hh"

using namespace std;

vector<string>
GCCModelGenerator::parse_linker_output( const vector<string> & linker_args )
{
  /* do we have a valid cache for these dependencies? */
  string command_line;
  for ( const auto & str : linker_args ) {
    command_line.append( str );
    command_line.append( 1, '\0' );
  }

  const auto cache_entry_path = gg::paths::dependency_cache_entry( digest::sha256( command_line ) );

  /* assemble the function */
  const gg::thunk::Function makedep_fn { linker_args.front(), linker_args, gcc_environment(), linker_args.front() };

  if ( roost::exists( cache_entry_path ) ) {
    /* abuse the thunk format to store a cache of dependencies */
    ThunkReader thunk_reader( cache_entry_path.string() );
    const gg::thunk::Thunk dep_cache_entry = thunk_reader.read_thunk();

    /* do we have a possible cache hit? */
    if ( makedep_fn == dep_cache_entry.function() ) {
      bool cache_hit = true;

      /* check if all the infiles are still the same */
      for ( const auto & infile : dep_cache_entry.infiles() ) {
        if ( not infile.matches_filesystem() ) {
          cache_hit = false;
          break;
        }
      }

      if ( cache_hit ) {
        vector<string> ret;
        for ( const auto & str : dep_cache_entry.infiles() ) {
          ret.emplace_back( str.filename() );
        }
        return ret;
      }
    }
  }

  istringstream linker_output { run( linker_args.front(), linker_args, {}, true, true, true, true ) };

  size_t seperator_line_count = 0;

  regex path_regex( "^(?:attempt to open|opened script file|found [^\\s]+ at) "
                    "([^\\s]+)\\s?(?:succeeded|)$" );
  smatch match;
  string line;

  unordered_set<string> dependencies;

  while ( getline( linker_output, line ) ) {
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

  const vector<string> the_dependencies { dependencies.begin(), dependencies.end() };

  /* write a cache entry for next time */

    /* assemble the infiles */
  vector<gg::thunk::InFile> infiles_vec;
  for ( const auto & str : the_dependencies ) {
    infiles_vec.emplace_back( str );
  }

  gg::thunk::Thunk dep_cache_entry( "",
                                    makedep_fn,
                                    infiles_vec );

  /* serialize and write the fake thunk */
  string serialized_cache_entry { gg::thunk::MAGIC_NUMBER };
  if ( not dep_cache_entry.to_protobuf().AppendToString( &serialized_cache_entry ) ) {
    throw runtime_error( "could not serialize cache entry" );
  }
  atomic_create( serialized_cache_entry, cache_entry_path );

  return the_dependencies;
}
