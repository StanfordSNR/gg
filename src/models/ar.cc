/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <string>
#include <cstring>
#include <getopt.h>

#include "timeouts.hh"
#include "thunk/factory.hh"
#include "thunk/ggutils.hh"
#include "thunk/thunk.hh"
#include "util/args.hh"
#include "util/exception.hh"
#include "util/path.hh"
#include "util/system_runner.hh"

#include "toolchain.hh"

using namespace std;
using namespace std::chrono;
using namespace gg::thunk;

static constexpr int PLUGIN_FLAG = 1000;

/* this function is based on ar source code */
void generate_thunk( int argc, char * argv[] )
{
  if ( argc < 2 ) {
    throw runtime_error( "not enough arguments" );
  }

  struct option long_options[] = {
    { "plugin", required_argument, nullptr, PLUGIN_FLAG },
    { 0, 0, 0, 0 },
  };

  bool need_relpos = false;
  bool need_count = false;

  /* this determines that we should treat [member] arguments of ar as infiles
     or no. For example, when ar is in delete mode, those members refer to the
     files inside the archive, not files on the file system. */
  bool members_infile = true;

  ExpandedArgs e_args = ExpandedArgs::expand( argc, argv );

  /* step three: convert the old format args to the new format */
  if ( argc > 1 and e_args.args[ 1 ][ 0 ] != '-' ) {
    for ( const char * letter = e_args.args[ 1 ]; *letter; letter++ ) {
      vector<char> new_str { '-', *letter, '\0' };
      e_args.arg_data.push_back( new_str );
      e_args.args.insert( e_args.args.begin() + 2, &e_args.arg_data.back()[ 0 ] );
    }

    e_args.args.erase( e_args.args.begin() + 1 );
  }

  argc = e_args.args.size();
  argv = &e_args.args[ 0 ];

  vector<string> original_args = gg::models::args_to_vector( argc, argv, program_data.at( AR ).filename() );

  vector<ThunkFactory::Data> data;

  optind = 1; /* reset getopt */
  opterr = 0; /* turn off error messages */

  while ( true ) {
    const int opt = getopt_long( argc, argv, "abiNpxtTdms", long_options, NULL );

    if ( opt == -1 ) {
      break;
    }

    switch ( opt ) {
    case 'a':
    case 'b':
    case 'i':
      need_relpos = true;
      break;

    case 'N':
      need_count = true;
      break;

    case 'p':
      throw runtime_error( "model for operation print not implemented" );

    case 'x':
      throw runtime_error( "model for operation extract not implemented" );

    case 't':
      throw runtime_error( "model for operation table not implemented" );

    case 'T':
      throw runtime_error( "model for thin archives not implemented" );

    case 'd':
    case 'm':
      members_infile = false;
      break;

    case PLUGIN_FLAG:
      data.emplace_back( optarg );
      break;
    }
  }

  string outfile;

  int i = optind + ( need_relpos ? 1 : 0 ) + ( need_count ? 1 : 0 );
  outfile = argv[ i++ ];

  if ( members_infile ) {
    for ( ; i < argc; i++ ) {
      data.emplace_back( argv[ i ] );
    }
  }

  /* XXX revisit this */
  if ( roost::exists( outfile ) ) {
    /* this means that the ar command might want change an existing library, so
    we have to list that as an infile */
    data.push_back( outfile );
  }

  ThunkFactory::generate(
    {
      program_hash( AR ),
      original_args,
      {}
    },
    data,
    { program_data.at( AR ) },
    { { "output", outfile } },
    {},
    AR_TIMEOUT,
    ThunkFactory::Options::create_placeholder
      | ThunkFactory::Options::collect_data
      | ThunkFactory::Options::generate_manifest
      | ThunkFactory::Options::include_filenames
  );
}

int main( int argc, char * argv[] )
{
  try {
    gg::models::init();
    generate_thunk( argc, argv );
  }
  catch ( const exception & e ) {
    print_exception( argv[ 0 ], e );
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
