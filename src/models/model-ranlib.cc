/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <boost/filesystem.hpp>
#include <getopt.h>

#include "thunk.hh"
#include "utils.hh"

#include "toolchain.hh"

using namespace std;
using namespace gg::thunk;
namespace fs = boost::filesystem;

static const string RANLIB = "ranlib";
static const fs::path toolchain_path { TOOLCHAIN_PATH };

Thunk generate_thunk( int argc, char * argv[] )
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

  return {
    archive,
    { RANLIB, gg::models::args_to_vector( argc, argv ), program_hash( RANLIB ) },
    {
      archive,
      { RANLIB, ( toolchain_path / RANLIB ).string(), program_hash( RANLIB ), 0 },
    }
  };
}

int main( int argc, char * argv[] )
{
  fs::path gg_dir = gg::models::create_gg_dir();

  Thunk thunk = generate_thunk( argc, argv );
  thunk.store( gg_dir );

  return 0;
}
