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
       << "\t[--output-dir, -d <dirname>]..." << endl
       << "\t[--output-path, -p <path>]..." << endl
       << "\t[--placeholder, -C <arg>]" << endl
       << "\t[--timeout, -T <arg (ms)>]" << endl
       << "\t[--link, -l <name>=<hash>]..." << endl
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
      { "output-dir",  required_argument, nullptr, 'd' },
      { "output-path", required_argument, nullptr, 'p' },
      { "placeholder", required_argument, nullptr, 'C' },
      { "timeout",     required_argument, nullptr, 'T' },
      { "link",        required_argument, nullptr, 'L' },
      { nullptr, 0, nullptr, 0 }
    };

    string placeholder_path;
    string function_hash;
    Optional<string> output_dir;
    string output_path;
    vector<string> function_args;
    vector<string> function_envars;
    vector<Thunk::DataItem> values;
    vector<Thunk::DataItem> thunks;
    vector<Thunk::DataItem> executables;
    vector<string> outputs;
    map<string, string> links;
    milliseconds timeout = 0s;

    while ( true ) {
      const int opt = getopt_long( argc, argv, "E:v:t:e:o:C:T:L:", cmd_options, nullptr );

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

      case 'd':
        output_dir.initialize(optarg);
        break;

      case 'p':
        output_path = optarg;
        break;

      case 'C':
        placeholder_path = optarg;
        break;

      case 'T':
        timeout = milliseconds { stoul( optarg ) };
        break;

      case 'L': {
        auto d = Thunk::string_to_data( optarg );

        if ( d.second.empty() ) {
          throw runtime_error( "missing link target" );
        }

        links[ d.first ] = d.second;
        break;
      }

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

    if ( output_dir.initialized() ) {
      thunk.set_ouput_dir( output_dir.get() );
    };

    for ( auto & link : links ) {
      thunk.add_link( link.first, link.second );
    }

    string thunk_hash = ThunkWriter::write( thunk, output_path );

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
