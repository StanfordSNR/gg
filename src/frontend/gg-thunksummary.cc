/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <unistd.h>
#include <cstring>
#include <iostream>

#include "thunk/ggutils.hh"
#include "thunk/thunk_reader.hh"
#include "thunk/thunk.hh"
#include "trace/syscall.hh"
#include "util/exception.hh"
#include "util/path.hh"

using namespace std;
using namespace gg::thunk;

void usage( const char * argv0 )
{
  cerr << argv0 << " THUNK-HASH" << endl;
}

string shortn( const string & hash )
{
  return hash.substr( 0, 5 );
}

struct ThunkStats
{
  std::unordered_map<std::string, size_t> files {};
  std::unordered_set<std::string> thunks {};

  ThunkStats & operator+=( const ThunkStats & other )
  {
    files.insert( other.files.begin(), other.files.end() );
    thunks.insert( other.thunks.begin(), other.thunks.end() );
    return *this;
  }

  size_t total_file_size() const
  {
    size_t ret = 0;
    for ( const auto & x : files ) {
      ret += x.second;
    }
    return ret;
  }
};

ThunkStats print_thunk_info( const string & hash, unsigned int indent )
{
  const Thunk thunk { move( ThunkReader::read( gg::paths::blob( hash ) ) ) };
  const string indentation( indent, ' ' );

  const string display_name = shortn( hash );

  ThunkStats stats;
  stats.thunks.insert( hash );

  cout << indentation << display_name << "\n";

  for ( const auto & item : thunk.values() ) {
    const string infile_name = shortn( item.first );
    cout << indentation << " " << infile_name << "\n";
    stats.files.insert( { item.first, gg::hash::size( item.first ) } );
  }

  for ( const auto & item : thunk.thunks() ) {
    const string infile_name = shortn( item.first );
    stats += print_thunk_info( item.first, indent + 1 );
  }

  const string plural = stats.thunks.size() > 1 ? "s" : "";

  cout << indentation << display_name << " uses " << stats.thunks.size()
       << " thunk" << plural << " and " << stats.files.size() << " files totaling "
       << stats.total_file_size() / 1048576 << " MiB\n";

  return stats;
}

int main( int argc, char * argv[] )
{
  try {
    if ( argc <= 0 ) {
      abort();
    }

    if ( argc != 2 ) {
      usage( argv[ 0 ] );
      return EXIT_FAILURE;
    }

    print_thunk_info( argv[ 1 ], 0 );
  }
  catch ( const exception &  e ) {
    print_exception( argv[ 0 ], e );
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
