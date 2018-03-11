/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <getopt.h>

#include "thunk/thunk.hh"
#include "thunk/ggutils.hh"
#include "util/path.hh"

#include "toolchain.hh"

using namespace std;
using namespace gg::thunk;

void generate_thunk( int argc, char * argv[] )
{
  if ( argc < 2 ) {
    throw runtime_error( "not enough arguments" );
  }

  struct option long_options[] = {
    { 0, 0, 0, 0 },
  };

  optind = 1; /* reset getopt */
  opterr = 0; /* turn off error messages */
  int opt;
  while ( ( opt = getopt_long( argc, argv, "", long_options, NULL ) ) != -1 ) {}

  string archive = argv[ optind ];

  ThunkFactory::generate(
    {
      program_hash( RANLIB ),
      gg::models::args_to_vector( argc, argv, program_data.at( RANLIB ).filename() ),
      {}
    },
    { archive, program_data.at( RANLIB ) },
    { { "output", archive } },
    true
  );
}

int main( int argc, char * argv[] )
{
  gg::models::init();
  generate_thunk( argc, argv );
  return 0;
}
