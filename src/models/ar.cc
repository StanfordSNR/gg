/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <string>
#include <cstring>
#include <getopt.h>
#include <wordexp.h>

#include "thunk/factory.hh"
#include "thunk/ggutils.hh"
#include "thunk/thunk.hh"
#include "thunk/metadata.hh"
#include "util/exception.hh"
#include "util/path.hh"
#include "util/system_runner.hh"

#include "toolchain.hh"

using namespace std;
using namespace gg::thunk;

const int PLUGIN_FLAG = 1000;

/* this function is based on ar source code */
void generate_thunk( int argc, char * argv[] )
{
  Optional<PlaceholderMetadata> metadata_ {};

  if ( gg::meta::metainfer() ) {
    metadata_.reset( gg::models::args_to_vector( argc, argv ),
                     gg::meta::relative_cwd().string() );
  }

  if ( argc < 2 ) {
    throw runtime_error( "not enough arguments" );
  }

  vector<string> original_args = gg::models::args_to_vector( argc, argv, program_data.at( AR ).filename() );

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

  /* step one: put all args in to this vector */
  char * const * in = argv;
  while ( in < argv + argc ) {
    new_argv.push_back( *in++ );
  }

  /* step two: expand all the args starting with @ */
  for ( size_t i = 0; i < new_argv.size(); i++ ) {
    if ( new_argv[ i ][ 0 ] == '@' ) {
      /* this needs to be expanded */
      roost::path file_path { new_argv[ i ] + 1 };

      if ( not roost::exists( file_path ) ) {
        /* leave it be */
        continue;
      }

      const string content = roost::read_file( file_path );
      wordexp_t p;
      unique_ptr<wordexp_t, decltype( wordfree ) *> p_ptr { &p, wordfree };

      if ( wordexp( content.c_str(), p_ptr.get(), WRDE_NOCMD ) ) {
        throw runtime_error( "error while expanding argument" );
      }

      vector<char *> expanded_args;
      for ( size_t i = 0; i < p.we_wordc; i++ ) {
        vector<char> new_str;
        for ( const char * letter = p.we_wordv[ i ]; *letter; letter++ ) {
          new_str.push_back( *letter );
        }
        new_str.push_back( '\0' );
        argv_data.push_back( new_str );
        expanded_args.push_back( &argv_data.back()[ 0 ] );
      }

      new_argv[ i ] = expanded_args.back();
      new_argv.insert( new_argv.begin() + i, expanded_args.begin(), expanded_args.end() - 1 );
      i--;
    }
  }

  /* step three: convert the old format args to the new format */
  if ( argc > 1 and argv[ 1 ][ 0 ] != '-' ) {
    for ( const char * letter = argv[ 1 ]; *letter; letter++ ) {
      vector<char> new_str { '-', *letter, '\0' };
      argv_data.push_back( new_str );
      new_argv.insert( new_argv.begin() + 2, &argv_data.back()[ 0 ] );
    }

    new_argv.erase( new_argv.begin() + 1 );
  }

  argc = new_argv.size();
  argv = &new_argv[ 0 ];

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
      if ( metadata_.initialized() ) { metadata_->add_object( ThunkFactory::Data( optarg ) ); }
      break;
    }
  }

  string outfile;

  int i = optind + ( need_relpos ? 1 : 0 ) + ( need_count ? 1 : 0 );
  outfile = argv[ i++ ];

  if ( members_infile ) {
    for ( ; i < argc; i++ ) {
      data.emplace_back( argv[ i ] );
      if ( metadata_.initialized() ) { metadata_->add_object( ThunkFactory::Data( argv[ i ] ) ); }
    }
  }

  /* XXX revisit this */
  if ( roost::exists( outfile ) ) {
    /* this means that the ar command might want change an existing library, so
    we have to list that as an infile */
    data.push_back( outfile );
    // if ( metadata_.initialized() ) { throw runtime_error( "ar: unhandled case for metadata" ); }
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
