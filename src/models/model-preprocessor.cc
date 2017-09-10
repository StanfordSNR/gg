/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "model-gcc.hh"

#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <fstream>
#include <boost/tokenizer.hpp>
#include <sys/fcntl.h>
#include <unistd.h>

#include "temp_file.hh"
#include "system_runner.hh"
#include "thunk.hh"
#include "ggpaths.hh"
#include "thunk_reader.hh"
#include "exception.hh"

using namespace std;
using namespace boost;

vector<string> write_dependencies_file( const string & output_name,
                                        const string & target_name,
                                        const vector<gg::thunk::InFile> & infiles )
{
  /* always write a canonical dependencies file to make sure it matches our parse */

  string output { target_name + ":" };
  vector<string> filenames;
  for ( const auto & infile : infiles ) {
    filenames.emplace_back( infile.filename() );
    output.append( " " );
    output.append( infile.filename() );
  }
  output.append( "\n" );

  atomic_create( output, roost::path( output_name ) );

  return filenames;
}

vector<string> GCCModelGenerator::parse_dependencies_file( const string & dep_filename,
                                                           const string & target_name )
{
  vector<string> dependencies;

  string target_str = target_name + ": ";

  ifstream depin { dep_filename };
  string line;
  bool found_target = false;

  while ( getline( depin, line ) ) {
    if ( line.length() < 2 ) {
      if ( found_target ) { break; }
      else { continue; }
    }

    if ( not found_target and
         line.compare( 0, target_str.size(), target_str ) == 0 ) {
      line = line.substr( line.find( ':' ) + 1, line.length() );
      found_target = true;
    }
    else if ( found_target and
              line.find( ":" ) != string::npos ) {
      break;
    }

    if ( not found_target ) {
      continue;
    }

    line = line.substr( 1, line.length() );

    if ( line[ line.length() - 1 ] == '\\' ) {
      line = line.substr( 0, line.length() - 2 );
    }

    if ( line == "\\" ) {
      continue;
    }

    tokenizer<escaped_list_separator<char>> tok( line, { "\\", " ", "\"\'" } );

    for ( auto t = tok.begin(); t != tok.end(); t++ ) {
      dependencies.push_back( *t );
    }
  }

  return dependencies;
}

vector<string> GCCModelGenerator::generate_dependencies_file( const string & input_filename,
                                                              const vector<string> & option_args,
                                                              const string & output_name,
                                                              const string & target_name )
{
  vector<string> args;
  args.reserve( 1 + option_args.size() );

  if ( operation_mode_ == OperationMode::GCC ) {
    args.push_back( "gcc-7" );
  }
  else {
    args.push_back( "g++-7" );
  }

  args.insert( args.end(), option_args.begin(), option_args.end() );

  string output_filename;

  const bool has_dependencies_option = find_if(
    args.begin(), args.end(),
    []( const string & opt )
    {
      return ( opt == "-M" ) or ( opt == "-MF" ) or ( opt == "-MM" ) or
             ( opt == "-MG" ) or ( opt == "-MP" ) or ( opt == "-MQ" ) or
             ( opt == "-MD" ) or ( opt == "-MMD" );
    } ) != end( args );

  if ( has_dependencies_option ) {
    auto m_search = find( args.begin(), args.end(), "-M" );
    auto mf_search = find( args.begin(), args.end(), "-MF" );
    auto md_search = find( args.begin(), args.end(), "-MD" );

    if ( mf_search == end( args ) ) {
      throw runtime_error( "cannot produce dependencies file without -MF option" );
    }

    if ( md_search != end( args ) ) {
      args.erase( md_search );

      if ( m_search != end( args ) ) {
        args.push_back( "-M" );
      }
    }
  }
  else {
    args.push_back( "-M" );
    args.push_back( "-MF" );
    args.push_back( output_name );
    args.push_back( "-MT" );
    args.push_back( target_name );
  }

  /* do we have a valid cache for these dependencies? */
  const string input_file_hash = gg::thunk::InFile::compute_hash( input_filename );
  const auto cache_entry_path = gg::paths::dependency_cache_entry( input_file_hash );

  /* assemble the function */
  const gg::thunk::Function makedep_fn { args.front(), args, gcc_environment(), args.front() };

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
        return write_dependencies_file( output_name, target_name, dep_cache_entry.infiles() );
      }
    }
  }

  run( args[ 0 ], args, {}, true, true );

  /* write a cache entry for next time */

  /* assemble the infiles */
  const vector<string> infiles_list = parse_dependencies_file( output_name, target_name );
  vector<gg::thunk::InFile> dependencies;
  for ( const auto & str : infiles_list ) {
    dependencies.emplace_back( str );
  }

  gg::thunk::Thunk dep_cache_entry( "",
                                    makedep_fn,
                                    dependencies );

  /* serialize and write the fake thunk */
  string serialized_cache_entry { gg::thunk::MAGIC_NUMBER };
  if ( not dep_cache_entry.to_protobuf().AppendToString( &serialized_cache_entry ) ) {
    throw runtime_error( "could not serialize cache entry" );
  }
  atomic_create( serialized_cache_entry, cache_entry_path );

  return write_dependencies_file( output_name, target_name, dep_cache_entry.infiles() );
}
