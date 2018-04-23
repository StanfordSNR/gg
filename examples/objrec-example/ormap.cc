/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>
#include <utility>

#include "thunk/thunk.hh"
#include "thunk/thunk_writer.hh"
#include "util/path.hh"
#include "util/util.hh"
#include "thunk/ggutils.hh"

using namespace std;
using namespace gg;
using namespace gg::thunk;

const string objsearch [] = { "volcano", "stone wall", "monastery" };
const string running_list_name = "all_list.txt";

int main( int argc, char * argv[] ) {
  if ( argc < 4 ) {
    cout << "Usage: " << argv[0] << " file1 [file2, ...] <index> <new-list>" << endl;
    return EXIT_FAILURE;
  }

  int32_t index = atoi( argv[argc - 2] );
  string next_inp = argv[1 + index];  
  string new_list_name = argv[argc - 1];

  const string ormap_hash = safe_getenv( "ORMAP_FUNCTION_HASH" );
  const string do_merge_hash = safe_getenv( "DO_MERGE_FUNCTION_HASH" );
  const string has_wall_hash = safe_getenv( "HAS_WALL_FUNCTION_HASH" );

  /* create merge thunk */
  if ( index >= ( argc - 3 ) ) {
    vector<string> args = { "do_merge" };
    vector<pair<const string, string>> thunks;
    ifstream list_fs( new_list_name );
    string next_hash;

    while ( list_fs >> next_hash ) {
      args.push_back( thunk::data_placeholder( next_hash ) );
      thunks.push_back( make_pair( next_hash, "" ) );
    }
    list_fs.close();

    const Thunk do_merge_thunk {
      { do_merge_hash, args, {} },
      thunks,
      { { do_merge_hash, "" } },
      { { "all_found.out" } }
    };

    ThunkWriter::write( do_merge_thunk, "next" );

    /* Empty outputs */
    ofstream next_fs( "job", ofstream::out );
    next_fs.close();
    ofstream new_list ( running_list_name, ofstream::out );
    new_list.close();

    return EXIT_SUCCESS;
  }

  /* read file and determine if a new thunk should be created */
  ifstream infile( next_inp );
  string line;
  bool make_thunk = false;
  while ( getline( infile, line ) && !make_thunk ) {
    string object = line.substr( 0, line.find( " (" ) );
    auto search_obj = find( begin(objsearch), end(objsearch), object );
    if ( search_obj != end(objsearch) ) {
      make_thunk = true;
      break;
    }
  }

  /* Create a new thunk if needed. Need to create empty list otherwise. */
  if ( make_thunk ) {
    size_t last_slash = next_inp.find_last_of( "/\\" );
    string has_wall_inp = next_inp.substr( last_slash + 1 );
    const Thunk has_wall_thunk {
      { has_wall_hash, { "has_wall",
                         thunk::data_placeholder( has_wall_inp ) }, {} },
      { { has_wall_inp, "" } },
      { { has_wall_hash, "" } },
      { { "was_found.out" } }
    };

    const string hw_thunk_hash = ThunkWriter::write( has_wall_thunk, "job" );

    /* Add thunk to the list */
    ifstream orig_list ( new_list_name );
    ofstream new_list ( running_list_name, ofstream::out );

    string next_line;
    while ( orig_list >> next_line ) {
      new_list << next_line << '\n';
    }
    new_list << hw_thunk_hash << '\n';
    orig_list.close();
    new_list.close();
  }
  else {
    /* Create empty job */
    ofstream job_fs( "job", ofstream::out );
    job_fs.close();

    /* Copy the list to the output */
    ifstream orig_list ( new_list_name );
    ofstream new_list ( running_list_name, ofstream::out );
    string next_line;
    while ( orig_list >> next_line ) {
      new_list << next_line << '\n';
    }
    orig_list.close();
    new_list.close();
  }

  /* Create next ormap job */
  vector <string> envars = { "ORMAP_FUNCTION_HASH=" + ormap_hash,
                             "HAS_WALL_FUNCTION_HASH=" + has_wall_hash,
                             "DO_MERGE_FUNCTION_HASH=" + do_merge_hash };
  vector<string> args = { "ormap" };
  vector<pair<const string, string>> vals;

  /* Hash the list */
  string list_hash = gg::hash::file( running_list_name );

  vals.push_back( make_pair( list_hash, string() ) );

  for ( uint8_t i = 1; i < argc - 2; ++i ) {
    string next_file = argv[i];
    size_t last_slash = next_file.find_last_of( "/\\" );
    args.push_back( thunk::data_placeholder( next_file.substr( last_slash + 1 ) ) );
    vals.push_back( make_pair( next_file.substr( last_slash + 1 ), string() ) );
  }
  args.push_back( to_string( index + 1 ) );
  args.push_back( thunk::data_placeholder( list_hash ) );
  const Thunk next_ormap {
    { ormap_hash, args, envars },
    vals,
    { { ormap_hash, "" } },
    { { "next" }, { "job" }, { running_list_name } }
  };

  const string next_ormap_hash = ThunkWriter::write( next_ormap, "next" );

  return EXIT_SUCCESS;
}

