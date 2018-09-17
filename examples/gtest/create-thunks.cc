/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <chrono>
#include <algorithm>
#include <cctype>
#include <getopt.h>

#include "thunk/thunk.hh"
#include "thunk/factory.hh"
#include "thunk/placeholder.hh"
#include "thunk/ggutils.hh"
#include "util/exception.hh"
#include "util/path.hh"

using namespace std;
using namespace std::chrono;
using namespace gg::thunk;

static constexpr milliseconds TIMEOUT = 20s;

void usage( const char * argv0 )
{
  cerr << "Usage: " << argv0 << endl
       << "       " << "[-w|--gtest-wrapper=<path>] [-d|--data-path=<path>]" << endl
       << "       " << "[-b|--gtest-binary=<path>] [-e|--envar=<string>]..." << endl
       << "       " << "ANNOTATION-FILE" << endl;
}

struct TestCase
{
  string name {};
  vector<string> dependencies {};

  TestCase( const string & name, const vector<string> & top_level_deps,
            const vector<string> & deps )
    : name( name ), dependencies( top_level_deps )
  {
    dependencies.insert( dependencies.end(), deps.begin(), deps.end() );
  }

  string str() const
  {
    ostringstream oss;
    oss << name << " [";
    for ( size_t i = 0; i < dependencies.size(); i++ ) {
      oss << dependencies[ i ];
      if ( i != dependencies.size() - 1 ) {
        oss << ", ";
      }
    }
    oss << "]";

    return oss.str();
  }

  string generate_thunk( const roost::path & gtest_wrapper,
                         const roost::path & data_path,
                         const roost::path & gtest_binary,
                         const vector<string> & envars ) const
  {
    vector<ThunkFactory::Data> values;
    vector<ThunkFactory::Data> executables;
    vector<ThunkFactory::Output> outputs { { "output" } };

    for ( const auto & dep : dependencies ) {
      values.emplace_back( dep, ( data_path / dep ).string() );
    }

    executables.emplace_back( "gtest-wrapper", gtest_wrapper.string() );
    executables.emplace_back( "gtest-binary", gtest_binary.string() );

    vector<string> args;
    args.emplace_back( "gtest-wrapper" );
    args.emplace_back( data_placeholder( gg::hash::file( gtest_binary ) ) );
    args.emplace_back( name );

    return ThunkFactory::generate(
      { executables.front().hash(), args, envars },
      values,
      executables,
      outputs,
      {},
      TIMEOUT,
      ThunkFactory::Options::collect_data
      | ThunkFactory::Options::include_filenames
    );
  }
};

static inline void rstrip( string & str )
{
  str.erase( find_if( str.rbegin(), str.rend(),
    []( int ch ) { return not isspace( ch ); } ).base(), str.end() );
}

vector<TestCase> parse_annotations_file( const roost::path & path )
{
  string top_level_name;
  vector<string> top_level_deps;
  string current_name;
  vector<string> current_deps;

  vector<TestCase> test_cases;

  ifstream fin { path.string() };
  string line;

  while ( getline( fin, line ) ) {
    const size_t hash_index = line.find( '#' );
    if ( hash_index != string::npos ) {
      line = line.substr( 0, hash_index );
    }
    rstrip( line );

    if ( not line.length() ) { continue; }

    if ( line[ 0 ] != ' ' ) {
      top_level_name = line;
      top_level_deps.clear();
      current_name.clear();
      current_deps.clear();
      continue;
    }

    if ( line.compare( 0, 3, "  @" ) == 0 ) {
      top_level_deps.push_back( line.substr( 3 ) );
    }
    else if ( line.compare( 0, 5, "    @" ) == 0 ) {
      current_deps.push_back( line.substr( 5 ) );
    }
    else if ( line.compare( 0, 2, "  " ) == 0 ) {
      if ( current_name.length() > 0 ) {
        const string fullname = top_level_name + current_name;
        if ( fullname.find( "DISABLED" ) == string::npos ) {
          test_cases.emplace_back( top_level_name + current_name, top_level_deps, current_deps );
        }
      }

      current_name = line.substr( 2 );
      current_deps.clear();
    }
  }

  return test_cases;
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

    gg::models::init();

    roost::path annotations_file;
    roost::path gtest_wrapper { "gtest-wrapper" };
    roost::path data_path { "." };
    roost::path gtest_binary { "run_test" };
    vector<string> envars;

    struct option long_options[] = {
      { "gtest-wrapper", required_argument, nullptr, 'w' },
      { "data-path", required_argument, nullptr, 'd' },
      { "gtest-binary", required_argument, nullptr, 'b' },
      { "envar", required_argument, nullptr, 'e' },
      { nullptr, 0, nullptr, 0 }
    };

    while ( true ) {
      const int opt = getopt_long( argc, argv, "w:b:d:e:", long_options, NULL );

      if ( opt == -1 ) {
        break;
      }

      switch ( opt ) {
      case 'w': gtest_wrapper = optarg; break;
      case 'd': data_path = optarg; break;
      case 'b': gtest_binary = optarg; break;
      case 'e': envars.emplace_back( optarg ); break;
      default: throw runtime_error( "unkown option" );
      }
    }

    if ( optind >= argc ) {
      usage( argv[ 0 ] );
      return EXIT_FAILURE;
    }

    annotations_file = argv[ optind++ ];

    if ( not roost::exists( annotations_file ) ) {
      throw runtime_error( "could not find annotations file at '" +
                           annotations_file.string() + "'" );
    }

    if ( not roost::exists( gtest_wrapper ) ) {
      throw runtime_error( "could not find gtest wrapper at '" +
                           gtest_wrapper.string() + "'" );
    }

    if ( not roost::exists( data_path ) or not roost::is_directory( data_path ) ) {
      throw runtime_error( "could not find data path at '" +
                           gtest_wrapper.string() + "'" );
    }

    if ( not roost::exists( gtest_binary ) ) {
      throw runtime_error( "could not find gtest binary at '"
                           + gtest_binary.string() + "'" );
    }

    vector<TestCase> test_cases = parse_annotations_file( annotations_file );

    for ( size_t i = 0; i < test_cases.size(); i++ ) {
      const string hash = test_cases[ i ].generate_thunk( gtest_wrapper, data_path, gtest_binary, envars );
      cout << "[" << ( i + 1 ) << "/" << test_cases.size() << "] " << hash << '\n';
      ThunkPlaceholder placeholder { hash };
      placeholder.write( to_string( i ) + ".test" );
    }
  }
  catch ( const exception &  e ) {
    print_exception( argv[ 0 ], e );
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
