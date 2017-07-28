/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "model-gcc.hh"

#include <vector>
#include <string>
#include <fstream>
#include <boost/tokenizer.hpp>

#include "temp_file.hh"
#include "system_runner.hh"

using namespace std;
using namespace boost;

vector<string> get_preprocess_dependencies( const vector<string> & gcc_args,
                                            const string & specsfile )
{
  vector<string> args;
  args.reserve( 2 + gcc_args.size() );
  args.push_back( "gcc-7" );
  args.push_back( "-specs=" + specsfile );
  args.insert( args.end(), gcc_args.begin(), gcc_args.end() );

  string dep_out_filename;

  auto has_dependencies_option = find_if(
    args.begin(), args.end(),
    []( const string & opt )
    {
      return ( opt == "-M" ) or ( opt == "-MF" ) or ( opt == "-MM" ) or
             ( opt == "-MG" ) or ( opt == "-MP" ) or ( opt == "-MQ" ) or
             ( opt == "-MD" ) or ( opt == "-MMD" );
    }
  );

  if ( has_dependencies_option != args.end() ) {
    throw runtime_error( "find dependencies: command already has -M flag" );
  }

  {
    UniqueFile gcc_mf_output { "/tmp/gg-model-gcc-mf" };
    dep_out_filename = gcc_mf_output.name();

    /* XXX we should probably get rid of -o option */
    args.push_back( "-M" );
    args.push_back( "-MF" );
    args.push_back( dep_out_filename );
  }

  run( args[ 0 ], args, {}, true, true );

  vector<string> dependencies;

  ifstream depin { dep_out_filename };
  string line;
  bool first_line = true;

  while ( getline( depin, line ) ) {
    if ( first_line ) {
      line = line.substr( line.find(':') + 2, line.length() );
      first_line = false;
    }
    else {
      line = line.substr( 1, line.length() );
    }

    if ( line[ line.length() - 1 ] == '\\' ) {
      line = line.substr( 0, line.length() - 2 );
    }

    if ( line == "\\" ) {
      continue;
    }

    tokenizer<escaped_list_separator<char>> tok( line, { "\\", " ", "\"\'" } );

    for ( auto t = tok.begin(); t != tok.end(); t++ ) {
      dependencies.push_back( *t );
    }
  }

  return dependencies;
}
