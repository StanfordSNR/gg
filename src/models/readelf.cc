/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <getopt.h>

#include "thunk/factory.hh"
#include "thunk/ggutils.hh"
#include "thunk/placeholder.hh"
#include "thunk/thunk.hh"
#include "util/exception.hh"
#include "util/system_runner.hh"
#include "util/path.hh"

#include "toolchain.hh"

using namespace std;
using namespace gg::thunk;

void write_to_stdout( int argc, char * argv[] )
{
  if ( argc < 2 ) {
    throw runtime_error( "not enough arguments" );
  }

  struct option long_options[] = {
    { 0, 0, 0, 0 },
  };

  vector<ThunkFactory::Data> data;

  optind = 1; /* reset getopt */
  opterr = 0; /* turn off error messages */
  int opt;
  while ( ( opt = getopt_long( argc, argv, "-d", long_options, NULL ) ) != -1 ) {
    if ( opt == 1 ) {
      data.emplace_back( optarg );
      continue;
    }

    switch ( opt ) {
    case 'd':
      break;

    default:
      throw runtime_error( "unsupported option" );
    }
  }

  if ( data.size() == 0 ) {
    throw runtime_error( "no inputs" );
  }

  cout << "# output from gg wrapper for readelf" << endl;
  cout << "readelf " << command_str( argc - 1, argv + 1 ) << endl;

  cout << program_data.at( READELF ).hash() << "=" << "readelf" << endl;
  for ( const auto & d : data ) {
    cout << d.hash() << "=" << d.filename() << endl;
  }
}

int main( int argc, char * argv[] )
{
  try {
    gg::models::init();
    write_to_stdout( argc, argv );
  }
  catch ( const exception & e ) {
    print_exception( argv[ 0 ], e );
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
