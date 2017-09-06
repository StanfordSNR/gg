/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <string>
#include <cstring>
#include <getopt.h>

#include "thunk.hh"
#include "ggpaths.hh"
#include "path.hh"

#include "toolchain.hh"

using namespace std;
using namespace gg::thunk;

Thunk generate_thunk( int argc, char * argv[] )
{
  if ( argc < 2 ) {
    throw runtime_error( "not enough arguments" );
  }

  vector<string> original_args = gg::models::args_to_vector( argc, argv );

  struct option long_options[] = {
    { 0, 0, 0, 0 },
  };

  optind = 1; /* reset getopt */
  opterr = 0; /* turn off error messages */

  while ( true ) {
    const int opt = getopt_long( argc, argv, "", long_options, NULL );

    if ( opt == -1 ) {
      break;
    }

    switch ( opt ) {
    default:
      throw runtime_error( "unknown option: " + string( argv[ optind - 1 ] ) );
    }
  }

  string outfile;
  vector<InFile> infiles;

  return { outfile,
    { LD, original_args, {}, program_data( LD ).first },
    infiles
  };
}

int main( int argc, char * argv[] )
{
  Thunk thunk = generate_thunk( argc, argv );
  thunk.store();

  return 0;
}
