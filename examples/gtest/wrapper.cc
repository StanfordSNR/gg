/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <string>
#include <vector>

#include "thunk/thunk.hh"
#include "thunk/thunk_reader.hh"
#include "util/exception.hh"
#include "util/path.hh"
#include "util/system_runner.hh"
#include "util/util.hh"

using namespace std;
using namespace gg::thunk;

void usage( const char * argv0 )
{
  cerr << argv0 << " TEST-BINARY TEST-NAME" << endl;
}

class TempSymlink
{
private:
  roost::path linkpath_;

public:
  TempSymlink( const roost::path & target, const roost::path & linkpath )
    : linkpath_( linkpath ) { roost::symlink( target, linkpath ); }

  ~TempSymlink()
  {
    try {
      roost::remove( linkpath_ );
    }
    catch ( const exception & e ) {
      print_exception( "TempSymlink", e );
    }
  }
};

int main( int argc, char * argv [] )
{
  try {
    if ( argc <= 0 ) {
      abort();
    }

    if ( argc != 3 ) {
      usage( argv[ 0 ] );
      return EXIT_FAILURE;
    }

    if ( getenv( "__GG_ENABLED__" ) == nullptr ) {
      throw runtime_error( "gg is not enabled" );
    }

    const roost::path thunk_path = safe_getenv( "__GG_THUNK_PATH__" );
    const roost::path gg_path = safe_getenv( "__GG_DIR__" );
    const Thunk thunk = ThunkReader::read( thunk_path );
    vector<TempSymlink> symlinks;
    symlinks.reserve( thunk.values().size() );

    /* (1) let's create the necessary symlinks */
    for ( const auto & value : thunk.values() ) {
      if ( value.second.length() > 0 ) {
        if ( value.second.find( '/' ) != string::npos ) {
          throw runtime_error( "paths are not supported yet" );
        }

        symlinks.emplace_back( gg_path / value.first, value.second );
      }
    }

    /* (2) run the test */
    const string test_binary { argv[ 1 ] };
    const string test_name { argv[ 2 ] };
    const string output_name { "output" };

    vector<string> all_args { argv[ 1 ] };
    all_args.push_back( "--gtest_filter=" + test_name );
    all_args.push_back( "--gtest_output=json:" + output_name );
    all_args.push_back( "--gtest_color=no" );

    run( all_args[ 0 ], all_args, {}, true, false );
  }
  catch ( const exception &  e ) {
    print_exception( argv[ 0 ], e );
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
