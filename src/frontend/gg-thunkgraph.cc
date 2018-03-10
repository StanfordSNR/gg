/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <unistd.h>
#include <cstring>
#include <iostream>

#include "util/exception.hh"
#include "trace/syscall.hh"
#include "thunk/thunk.hh"
#include "thunk/thunk_reader.hh"
#include "thunk/ggutils.hh"
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

void print_thunk_info( const string & hash )
{
  const Thunk thunk { ThunkReader { gg::paths::blob_path( hash ).string() }.read_thunk() };

  //  cout << indentation << shortnhash << ": " << thunk.function().exe() << " -> " << thunk.outfile() << "\n";

  const string display_name = roost::rbasename( thunk.outfile() ).string() + "=" + shortn( hash );

  for ( const auto & infile : thunk.infiles() ) {
    const string infile_name = roost::rbasename( infile.filename() ).string() + "=" + shortn( infile.content_hash() );
    if ( infile.order() ) {
      cout << "\"" << display_name << "\" -> \"" << infile_name << "\"\n";
      print_thunk_info( infile.content_hash() );
    } else if ( infile.size() ) {
      if ( not infile.filename().empty() and infile.filename().front() != '/' ) {
        cout << "\"" << display_name << "\" -> \"" << infile_name << "\"\n";
      }
    }
  }
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

    cout << "digraph \"" << argv[ 1 ] << "\" {\n";
    cout << "overlap = false;\n";
    cout << "splines = true;\n";
    print_thunk_info( argv[ 1 ] );
    cout << "}\n";
  }
  catch ( const exception &  e ) {
    print_exception( argv[ 0 ], e );
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
