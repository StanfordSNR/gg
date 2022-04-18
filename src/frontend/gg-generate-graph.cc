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
  return hash.substr( 0, 6 );
}

void print_thunk_graph( const string & hash )
{
  const static string INDENT( 2, ' ' );
  const Thunk thunk { move( ThunkReader::read( gg::paths::blob( hash ) ) ) };

  const string display_name = shortn( hash );

  cout << INDENT << '"' << display_name << '"'
        << " [label=\"" << display_name << "\"]\n";

  size_t total_value_size = 0;
  
  for ( const auto & item : thunk.values() ) {
    total_value_size += gg::hash::size( item.first );
  }

  for ( const auto & item : thunk.executables() ) {
    total_value_size += gg::hash::size( item.first );
  }

  if ( total_value_size ) {
    cout << INDENT << '"' << display_name << "_inputs\" [shape=oval]\n";
    cout << INDENT << '"' << display_name << "\" -> \"" << display_name << "_inputs\""
         << " [label=\"" << total_value_size << "\",style=dashed]\n";
  }

  for ( const auto & item : thunk.thunks() ) {
    const string infile_name = shortn( item.first );
    
    Optional<gg::cache::ReductionResult> cached_result;
    cached_result.initialize( gg::cache::ReductionResult { item.first } );

    while ( true ) {
      cached_result = gg::cache::check( cached_result->hash );

      if ( !cached_result.initialized() ) {
        throw runtime_error( "thunk result not found: " + item.first );
      }
      
      if ( gg::hash::type( cached_result->hash ) == gg::ObjectType::Value ) {
        break;
      }
    }

    size_t infile_size = gg::hash::size( cached_result->hash );

    print_thunk_graph( item.first );
    cout << INDENT << '"' << display_name << "\" -> \"" << infile_name << '"'
         << " [label=\"" << infile_size << "\"]\n";
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

    cout << "digraph {\n\n"
         << "  node [shape=box]\n"
         << "  edge [arrowhead=vee,arrowtail=vee,arrowsize=0.5]\n\n";
    print_thunk_graph( argv[ 1 ] );
    cout << "\n}\n";
  }
  catch ( const exception &  e ) {
    print_exception( argv[ 0 ], e );
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
