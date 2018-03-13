/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "model-gcc.hh"

#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <unordered_set>

#include "thunk/factory.hh"
#include "util/system_runner.hh"

using namespace std;
using namespace gg::thunk;

const bool include_gompspec = ( getenv( "GG_GCC_OPENMP_SUPPORT" ) != nullptr );

vector<string> GCCModelGenerator::get_link_dependencies( const vector<InputFile> & link_inputs,
                                                         const vector<string> & gcc_args )
{
  unordered_set<string> dependencies;
  vector<string> args { gcc_args };

  size_t last_index = SIZE_MAX;
  for ( auto it = link_inputs.rbegin(); it != link_inputs.rend(); it++ ) {
    if ( it->language == Language::SHARED_LIBRARY or
         it->language == Language::SHARED_OBJECT ) {
      continue;
    }

    assert( it->index < last_index );
    args.erase( args.begin() + it->index );
    last_index = it->index;
  }

  args.erase( remove( args.begin(), args.end(), string( "-Wl,--as-needed" ) ), args.end() );

  if ( operation_mode_ == OperationMode::GCC ) {
    args.insert( args.begin(), ( toolchain_path / GCC ).string() );
  }
  else {
    args.insert( args.begin(), ( toolchain_path / GXX ).string() );
  }

  args.push_back( "-Wl,--verbose" );
  args.push_back( "-B" + gcc_install_path );

  return parse_linker_output( args );
}


string GCCModelGenerator::generate_link_thunk( const vector<InputFile> & link_inputs,
                                               const vector<string> & dependencies,
                                               const string & output )
{
  vector<ThunkFactory::Data> infiles;
  vector<string> dummy_dirs;

  if ( operation_mode_ == OperationMode::GCC ) {
    infiles.emplace_back( program_data.at( GCC ) );
  }
  else {
    infiles.emplace_back( program_data.at( GXX ) );
  }

  infiles.emplace_back( program_data.at( COLLECT2 ) );
  infiles.emplace_back( program_data.at( LD ) );

  for ( auto const & link_input : link_inputs ) {
    if ( link_input.language == Language::OBJECT or
         link_input.language == Language::SHARED_OBJECT or
         link_input.language == Language::ARCHIVE_LIBRARY ) {
      infiles.push_back( link_input.indata );
    }
  }

  for ( const string & dep : dependencies ) {
    infiles.emplace_back( dep );
  }

  /* ARGS */
  vector<string> args { arguments_.option_args() };
  args.push_back( "-o" );
  args.push_back( output );

  for ( const InputFile & input : link_inputs ) {
    switch ( input.language ) {
    case Language::OBJECT:
    case Language::SHARED_OBJECT:
    case Language::ARCHIVE_LIBRARY:
      args.push_back( input.name );
      break;

    case Language::SHARED_LIBRARY:
      args.push_back( "-l" + input.name );
      break;

    default:
      throw runtime_error( "invalid input for link stage" );
    }
  }

  vector<string> all_args;
  all_args.reserve( args.size() + gcc_library_path.size() );

  if ( not arguments_.no_stdlib() ) {
    for ( const string & dir : gcc_library_path ) {
      dummy_dirs.push_back( dir );
      all_args.push_back( "-L" + dir );
    }
  }

  for ( const string & dir : arguments_.library_dirs() ) {
    dummy_dirs.push_back( dir );
    all_args.push_back( "-L" + dir );
  }

  all_args.insert( all_args.end(), args.begin(), args.end() );

  all_args.push_back( "-B" + gcc_install_path );

  for ( const string & dir : ld_search_path ) {
    dummy_dirs.push_back( dir );
    all_args.push_back( "-Wl,-rpath-link," + dir );
  }

  if ( arguments_.force_strip() ) {
    all_args.push_back( "-s" );
  }

  dummy_dirs.push_back( gcc_install_path );

  for ( const string & infile : arguments_.extra_infiles( LINK ) ) {
    infiles.emplace_back( infile );
  }

  if ( include_gompspec ) {
    /* let's look for libgomp.spec in library search path */
    bool found = false;
    for ( const string & dir : gcc_library_path ) {
      const roost::path spec_path = roost::path( dir ) / "libgomp.spec";
      if ( roost::exists( spec_path ) ) {
        infiles.emplace_back( spec_path.string() );
        found = true;
        break;
      }
    }

    if ( not found ) {
      throw runtime_error( "could not find file libgomp.spec" );
    }
  }

  return ThunkFactory::generate(
    gcc_function( operation_mode_, all_args, envars_ ),
    infiles,
    { { "output", output } },
    true, dummy_dirs, true
  );
}
