/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <unistd.h>
#include <cstring>
#include <iostream>

#include "exception.hh"
#include "syscall.hh"
#include "thunk.hh"
#include "thunk_reader.hh"
#include "ggpaths.hh"
#include "path.hh"

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

void print_thunk_info( const string & hash, unsigned int indent )
{
  const Thunk thunk { ThunkReader { gg::paths::blob_path( hash ).string() }.read_thunk() };
  const string indentation( indent, ' ' );
  
  const string display_name = roost::rbasename( thunk.outfile() ).string() + " (" + shortn( hash ) + ")";

  cout << indentation << display_name << " {" << thunk.function().exe() << "}\n";

  for ( const auto & infile : thunk.infiles() ) {
    const string infile_name = roost::rbasename( infile.filename() ).string() + " (" + shortn( infile.content_hash() ) + ")";
    if ( infile.order() ) {
      print_thunk_info( infile.content_hash(), indent + 1 );
    } else if ( infile.size() ) {
      cout << indentation << " " << infile_name << "\n";
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

    print_thunk_info( argv[ 1 ], 0 );
  }
  catch ( const exception &  e ) {
    print_exception( argv[ 0 ], e );
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
