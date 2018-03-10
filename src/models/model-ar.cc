/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <string>
#include <cstring>
#include <getopt.h>

#include "thunk/thunk.hh"
#include "thunk/ggutils.hh"
#include "util/path.hh"

#include "toolchain.hh"

using namespace std;
using namespace gg::thunk;

const int PLUGIN_FLAG = 1000;

/* this function is based on ar source code */
Thunk generate_thunk( int argc, char * argv[] )
{
  if ( argc < 2 ) {
    throw runtime_error( "not enough arguments" );
  }

  vector<string> original_args = gg::models::args_to_vector( argc, argv );

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

  vector<char *> new_argv;
  vector<vector<char>> argv_data;

  if ( argc > 1 and argv[ 1 ][ 0 ] != '-' ) {
    int new_argc;
    char * const * in; /* cursor into original argv */
    const char * letter; /* cursor into old option letters */

    new_argc = argc - 1 + strlen( argv[ 1 ] );

    in = argv;
    new_argv.push_back( *in++ );

    for ( letter = *in++; *letter; letter++ ) {
      vector<char> new_str { '-', *letter, '\0' };
      argv_data.push_back( new_str );
      new_argv.push_back( &argv_data.back()[ 0 ] );
    }

    while ( in < argv + argc ) {
      new_argv.push_back( *in++ );
    }

    argc = new_argc;
    argv = &new_argv[ 0 ];
  }

  vector<InFile> infiles;

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
      infiles.emplace_back( optarg );
      break;
    }
  }

  string outfile;

  infiles.push_back( program_infiles.at( AR ) );

  int i = optind + ( need_relpos ? 1 : 0 ) + ( need_count ? 1 : 0 );
  outfile = argv[ i++ ];

  if ( members_infile ) {
    for ( ; i < argc; i++ ) {
      infiles.emplace_back( argv[ i ] );
    }
  }

  /* XXX revisit this */
  if ( roost::exists( outfile ) ) {
    /* this means that the ar command might want change an existing library, so
    we have to list that as an infile */
    infiles.push_back( outfile );
  }

  return { outfile,
    { AR, original_args, {}, program_data( AR ).first },
    infiles
  };
}

int main( int argc, char * argv[] )
{
  gg::models::init();

  Thunk thunk = generate_thunk( argc, argv );
  thunk.store();

  return 0;
}
