/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <string>
#include <cstring>
#include <getopt.h>
#include <list>

#include "gcc.hh"
#include "timeouts.hh"
#include "thunk/factory.hh"
#include "thunk/ggutils.hh"
#include "thunk/thunk.hh"
#include "util/path.hh"

#include "toolchain.hh"

using namespace std;
using namespace gg::thunk;

template <typename E>
constexpr auto to_underlying( E e ) noexcept
{
    return static_cast<std::underlying_type_t<E>>( e );
}

enum class LDOption
{
  no_undefined = 1000, nostdlib, pie, start_group, end_group, whole_archive, no_whole_archive, build_id
};

vector<string> get_link_dependencies( size_t argc, char * argv[], list<size_t> input_indexes )
{
  vector<string> args;

  args.reserve( argc - input_indexes.size() );
  args.emplace_back( "ld" );

  for ( size_t i = 1; i < argc; i++ ) {
    if ( input_indexes.size() > 0 and input_indexes.front() == i ) {
      input_indexes.pop_front();
      continue;
    }

    if ( strcmp( argv[ i ], "--as-needed") != 0 ) {
      args.push_back( argv[ i ] );
    }
  }

  args.push_back( "--verbose" );

  assert( input_indexes.size() == 0 );

  return GCCModelGenerator::parse_linker_output( args );
}

void generate_thunk( size_t argc, char * argv[] )
{
  if ( argc < 2 ) {
    throw runtime_error( "not enough arguments" );
  }

  vector<string> original_args = gg::models::args_to_vector( argc, argv );

  struct option long_options[] = {
    { "entry",        required_argument, nullptr, 'e' },
    { "no-undefined", no_argument,       nullptr, to_underlying( LDOption::no_undefined ) },
    { "nostdlib",     no_argument,       nullptr, to_underlying( LDOption::nostdlib ) },
    { "output",       required_argument, nullptr, 'o' },
    { "relocatable",  no_argument,       nullptr, 'r' },
    { "emit-relocs",  no_argument,       nullptr, 'q' },
    { "script",       required_argument, nullptr, 'T' },
    { "pie",          no_argument,       nullptr, to_underlying( LDOption::pie ) },
    { "start-group",  no_argument,       nullptr, to_underlying( LDOption::start_group ) },
    { "end-group",    no_argument,       nullptr, to_underlying( LDOption::end_group ) },
    { "whole-archive",  no_argument,     nullptr, to_underlying( LDOption::whole_archive ) },
    { "no-whole-archive",  no_argument,  nullptr, to_underlying( LDOption::no_whole_archive ) },
    { "build-id",     optional_argument, nullptr, to_underlying( LDOption::build_id ) },

    { 0, 0, 0, 0 },
  };

  optind = 1; /* reset getopt */
  opterr = 0; /* turn off error messages */

  bool no_stdlib = false;
  vector<string> dep_args;

  string outfile;
  vector<ThunkFactory::Data> indata;
  vector<std::string> dummy_dirs;
  list<size_t> input_indexes;

  while ( true ) {
    const int opt = getopt_long_only( argc, argv, "-rql:o:e:m:z:T:", long_options, NULL );

    if ( opt == -1 ) {
      break;
    }

    if ( opt == 1 ) {
      indata.emplace_back( optarg );
      input_indexes.emplace_back( optind - 1 );
      continue;
    }

    bool processed = true;

    switch( opt ) {
    case 'o':
      outfile = optarg;
      break;

    case 'T':
      indata.emplace_back( optarg );
      break;

    case '?':
      processed = false;
    }

    if ( not processed ) {
      switch ( static_cast<LDOption>( opt ) ) {
      case LDOption::nostdlib:
        no_stdlib = true;
        break;

      case LDOption::start_group:
      case LDOption::end_group:
      case LDOption::whole_archive:
      case LDOption::no_whole_archive:
      case LDOption::build_id:
      case LDOption::no_undefined:
      case LDOption::pie:
        break;

      default:
        throw runtime_error( "unknown option: " + string( argv[ optind - 1 ] ) );
      }
    }
  }

  vector<string> dependencies = get_link_dependencies( argc, argv, input_indexes );
  for ( const string & dep : dependencies ) {
    indata.emplace_back( dep );
  }

  vector<string> all_args;
  all_args.push_back( program_data.at( LD ).filename() );

  if ( not no_stdlib ) {
    all_args.push_back( "-nostdlib" );

    for ( const string & dir : ld_search_path ) {
      all_args.push_back( "-L" + dir );
      all_args.push_back( "-rpath-link" );
      all_args.push_back( dir );
      dummy_dirs.push_back( dir );
    }
  }

  all_args.insert( all_args.end(), original_args.begin() + 1, original_args.end() );

  ThunkFactory::generate(
    {
      program_hash( LD ),
      all_args,
      {}
    },
    indata,
    { program_data.at( LD ) },
    { { "output", outfile } },
    dummy_dirs,
    LINK_TIMEOUT,
    ThunkFactory::Options::create_placeholder
      | ThunkFactory::Options::collect_data
      | ThunkFactory::Options::generate_manifest
      | ThunkFactory::Options::include_filenames
  );
}

int main( int argc, char * argv[] )
{
  gg::models::init();
  generate_thunk( argc, argv );
  return 0;
}
