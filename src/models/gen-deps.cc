/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <iostream>
#include <regex>
#include <string>
#include <vector>
#include <unordered_set>

#include "util/exception.hh"
#include "util/path.hh"

using namespace std;

const vector<string> cpp_include_path = {
  ".", /* current dir */
  "/usr/include/c++/7",
  "/usr/include/x86_64-linux-gnu/c++/7",
  "/usr/include/c++/7/backward",
  "/usr/lib/gcc/x86_64-linux-gnu/7/include",
  "/usr/local/include",
  "/usr/lib/gcc/x86_64-linux-gnu/7/include-fixed",
  "/usr/include/x86_64-linux-gnu",
  "/usr/include",
};

class DependencyFinder
{
private:
  unordered_set<string> processed_files_ {};
  vector<string> found_dependencies_ {};

  const vector<string> prefixes { "#include_next", "#include" };

  bool find_deps( const roost::path & filename ) {
    if ( processed_files_.count( filename.string() ) ) {
      return false;
    }

    processed_files_.insert( filename.string() );

    const string file_data = roost::read_file( filename );

    for ( size_t i = 0; i < file_data.size(); ) {
      while ( file_data[ i ] == ' ' ) { i++; }

      for ( const auto & prefix : prefixes ) {
        if ( file_data.compare( i, prefix.length(), prefix ) == 0 ) {
          i += prefix.length();
          while ( file_data[ i ] == ' ' ) { i++; }

          char closing_char;
          switch ( file_data[ i ] ) {
          case '"':
            closing_char = '"';
            break;

          case '<':
            closing_char = '>';
            break;

          default:
            throw runtime_error( string( "invalid syntax" ) + file_data[i] );
          }

          size_t closing_loc = file_data.find( closing_char, i + 1 );

          const string included_filename = file_data.substr( i + 1, closing_loc - i - 1 );
          cerr << included_filename << endl;

          for ( const auto & include_dir : cpp_include_path ) {
            const roost::path included_file { include_dir + "/" + included_filename };

            if ( roost::exists( included_file ) ) {
              if ( find_deps( included_file ) ) {
                found_dependencies_.push_back( included_file.string() );
              }

              break;
            }
          }

          break;
        }
      }

      i = file_data.find( '\n', i ) + 1;
    }

    return true;
  }

public:
  DependencyFinder( const roost::path & filename )
  {
    find_deps( filename );
  }

  const vector<string> & deps() { return found_dependencies_; }
};

int main( int argc, char * argv[] )
{
  try {
    if ( argc <= 0 ) {
      abort();
    }

    if ( argc != 2 ) {
      cerr << "gen-deps [FILENAME]" << endl;
      return EXIT_FAILURE;
    }

    DependencyFinder dep_finder { argv[ 1 ] };
    for ( const auto & dep : dep_finder.deps() ) {
      cout << dep << endl;
    }
  }
  catch ( const exception &  e ) {
    print_exception( argv[ 0 ], e );
    return EXIT_FAILURE;
  }
}
