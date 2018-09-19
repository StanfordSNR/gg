/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <iostream>
#include <chrono>
#include <getopt.h>

#include "thunk/thunk.hh"
#include "thunk/thunk_writer.hh"
#include "thunk/placeholder.hh"
#include "thunk/ggutils.hh"
#include "util/exception.hh"

using namespace std;
using namespace std::chrono;
using namespace google::protobuf::util;
using namespace gg::thunk;

void usage( const char * argv0 )
{
  cerr << argv0 << endl
       << "\t[--envar, -E <environment-varible>]..." << endl
       << "\t[--value, -v <value[=name]>]..." << endl
       << "\t[--thunk, -t <thunk[=name]>]..." << endl
       << "\t[--executable, -e <executable[=name]>]..." << endl
       << "\t[--output, -o <tag>]..." << endl
       << "\t[--placeholder, -C <arg>]" << endl
       << "\t[--timeout, -T <arg (ms)>]" << endl
       << "\tFUNCTION-HASH FUNCTION-ARG..." << endl;
}

int main( int argc, char * argv[] )
{
  try {
    if ( argc <= 0 ) {
      abort();
    }

    if ( argc < 2 ) {
      usage( argv[ 0 ] );
      return EXIT_FAILURE;
    }

    const option cmd_options[] = {
      { "envar",       required_argument, nullptr, 'E' },
      { "value",       required_argument, nullptr, 'v' },
      { "thunk",       required_argument, nullptr, 't' },
      { "executable",  required_argument, nullptr, 'e' },
      { "output",      required_argument, nullptr, 'o' },
      { "placeholder", required_argument, nullptr, 'C' },
      { "timeout",     required_argument, nullptr, 'T' },
      { nullptr, 0, nullptr, 0 }
    };

    string placeholder_path;
    string function_hash;
    vector<string> function_args;
    vector<string> function_envars;
    vector<Thunk::DataItem> values;
    vector<Thunk::DataItem> thunks;
    vector<Thunk::DataItem> executables;
    vector<string> outputs;
    milliseconds timeout = 0s;

    while ( true ) {
      const int opt = getopt_long( argc, argv, "E:v:t:e:o:C:T:", cmd_options, nullptr );

      if ( opt == -1 ) { break; }

      switch ( opt ) {
      case 'E':
        function_envars.emplace_back( optarg );
        break;

      case 'v':
        values.emplace_back( Thunk::string_to_data( optarg ) );
        break;

      case 't':
        thunks.emplace_back( Thunk::string_to_data( optarg ) );
        break;

      case 'e':
        executables.emplace_back( Thunk::string_to_data( optarg ) );
        break;

      case 'o':
        outputs.emplace_back( optarg );
        break;

      case 'C':
        placeholder_path = optarg;
        break;

      case 'T':
        timeout = milliseconds { stoul( optarg ) };
        break;

      default:
        throw runtime_error( "invalid option" );
      }
    }

    if ( optind >= argc ) {
      usage( argv[ 0 ] );
      return EXIT_FAILURE;
    }

    function_hash = argv[ optind++ ];

    for ( int i = optind; i < argc; i++ ) {
      function_args.emplace_back( argv[ i ] );
    }

    Function function { move( function_hash ), move( function_args ),
                        move( function_envars ) };

    Thunk thunk { move( function ), move( values ), move( thunks ),
                  move( executables ), move( outputs ) };

    thunk.set_timeout( timeout );

    string thunk_hash = ThunkWriter::write( thunk );

    if ( placeholder_path.size() ) {
      ThunkPlaceholder placeholder { thunk_hash };
      placeholder.write( placeholder_path );
    }

    cerr << thunk_hash << endl;
  }
  catch ( const exception &  e ) {
    print_exception( argv[ 0 ], e );
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
