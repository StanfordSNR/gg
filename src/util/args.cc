/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "args.hh"

#include <iostream>
#include <stdexcept>
#include <memory>
#include <wordexp.h>

#include "path.hh"

using namespace std;

bool ExpandedArgs::needs_expansion( const int argc, char * const * argv )
{
  for ( int i = 1; i < argc; i++ ) {
    if ( argv[ i ][ 0 ] == '@' ) {
      return true;
    }
  }

  return false;
}

vector<char> to_vector( const char * arg )
{
  vector<char> output;
  for ( const char * letter = arg; *letter; letter++ ) {
    output.push_back( *letter );
  }
  output.push_back( '\0' );
  return output;
}

ExpandedArgs ExpandedArgs::expand( const int argc, char * const * argv )
{
  ExpandedArgs output;

  /* first put all the args in a vector */
  char * const * in = argv;
  while ( in < argv + argc ) {
    output.arg_data.push_back( to_vector( *in++ ) );
    output.args.push_back( &output.arg_data.back()[ 0 ] );
  }

  /* now let's expand */
  for ( size_t i = 0; i < output.args.size(); i++ ) {
    if ( output.args[ i ][ 0 ] == '@' ) {
      /* this needs to be expanded */
      roost::path file_path { output.args[ i ] + 1 };

      if ( not roost::exists( file_path ) ) {
        /* leave it be */
        continue;
      }

      string content = roost::read_file( file_path );
      wordexp_t p;

      int triml;
      for ( triml = content.size() - 1; triml >= 0; triml-- ) {
        if ( content[ triml ] != ' ' and content[ triml ] != '\n' and content[ triml ] != '\r' ) {
          break;
        }
      }
      content.erase( triml + 1 );

      //unique_ptr<wordexp_t, decltype( wordfree ) *> p_ptr { &p, wordfree };

      int x = 0;
      if ( ( x = wordexp( content.c_str(), &p, WRDE_NOCMD ) ) ) {
        cerr << x << endl;
        throw runtime_error( "error while expanding argument" );
      }

      vector<char *> expanded_args;
      for ( size_t i = 0; i < p.we_wordc; i++ ) {
        vector<char> new_str;
        for ( const char * letter = p.we_wordv[ i ]; *letter; letter++ ) {
          new_str.push_back( *letter );
        }
        new_str.push_back( '\0' );
        output.arg_data.push_back( new_str );
        expanded_args.push_back( &output.arg_data.back()[ 0 ] );
      }
      wordfree( &p );

      if ( expanded_args.size() ) {
        output.args[ i ] = expanded_args.back();
        output.args.insert( output.args.begin() + i,
                            expanded_args.begin(), expanded_args.end() - 1 );
      }
      else {
        output.args.erase( output.args.begin() + i );
      }

      i--;
    }
  }

  return output;
}
