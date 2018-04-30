/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <getopt.h>

#include "thunk/metadata.hh"
#include "thunk/factory.hh"
#include "thunk/ggutils.hh"
#include "thunk/thunk.hh"
#include "util/path.hh"

#include "toolchain.hh"

using namespace std;
using namespace gg::thunk;

void generate_thunk( int argc, char * argv[] )
{
  if ( argc < 2 ) {
    throw runtime_error( "not enough arguments" );
  }

  Optional<PlaceholderMetadata> metadata_ {};

  if ( gg::meta::metainfer() ) {
    metadata_.reset( gg::models::args_to_vector( argc, argv ),
                     gg::meta::relative_cwd().string() );
  }

  struct option long_options[] = {
    { 0, 0, 0, 0 },
  };

  string strip_output;

  optind = 1; /* reset getopt */
  opterr = 0; /* turn off error messages */
  int opt;
  while ( ( opt = getopt_long( argc, argv, "o:wN:", long_options, NULL ) ) != -1 ) {
    switch ( opt ) {
    case 'o':
      strip_output = optarg;
      break;
    }
  }

  string stripf = argv[ optind ];
  if ( metadata_.initialized() ) { metadata_->add_object( stripf ); }

  if ( strip_output.length() == 0 ) {
    if ( metadata_.initialized() ) {
      throw runtime_error( "strip: unhandled case for metadata" );
    }

    strip_output = stripf;
  }

  ThunkFactory::generate(
    {
      program_hash( STRIP ),
      gg::models::args_to_vector( argc, argv, program_data.at( STRIP ).filename() ),
      {}
    },
    { stripf },
    { program_data.at( STRIP ) },
    { { "output", strip_output } },
    {},
    ThunkFactory::Options::create_placeholder
      | ThunkFactory::Options::collect_data
      | ThunkFactory::Options::generate_manifest
      | ThunkFactory::Options::include_filenames
      | ( ( metadata_.initialized() ) ? ThunkFactory::Options::write_metadata : 0 ),
    metadata_.initialized() ? metadata_->str() : string {}
  );
}

int main( int argc, char * argv[] )
{
  gg::models::init();
  generate_thunk( argc, argv );
  return 0;
}
