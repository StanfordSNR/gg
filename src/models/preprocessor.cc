/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "gcc.hh"

#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <fstream>
#include <boost/tokenizer.hpp>
#include <sys/fcntl.h>
#include <unistd.h>

#include "thunk/ggutils.hh"
#include "thunk/thunk_writer.hh"
#include "thunk/thunk_reader.hh"
#include "thunk/thunk.hh"
#include "util/exception.hh"
#include "util/system_runner.hh"
#include "util/temp_file.hh"

using namespace std;
using namespace boost;
using namespace gg::thunk;

class CouldNotParse : public runtime_error
{
public: using runtime_error::runtime_error;
};

vector<string> write_dependencies_file( const string & output_name,
                                        const string & target_name,
                                        const Thunk::DataList & data_items )
{
  /* always write a canonical dependencies file to make sure it matches our parse */
  string output { target_name + ":" };
  vector<string> filenames;
  for ( const auto & item : data_items ) {
    filenames.emplace_back( item.second );
    output.append( " " );
    output.append( item.second );
  }
  output.append( "\n" );

  roost::atomic_create( output, roost::path( output_name ) );
  return filenames;
}

vector<string> write_dependencies_file( const string & output_name,
                                        const string & target_name,
                                        const vector<string> & files )
{
  /* always write a canonical dependencies file to make sure it matches our parse */
  string output { target_name + ":" };
  vector<string> filenames;
  for ( const auto & item : files ) {
    filenames.emplace_back( item );
    output.append( " " );
    output.append( item );
  }
  output.append( "\n" );

  roost::atomic_create( output, roost::path( output_name ) );
  return filenames;
}


vector<string> GCCModelGenerator::parse_dependencies_file( const string & dep_filename,
                                                           const string & target_name )
{
  vector<string> dependencies;

  string target_str = target_name + ": ";

  ifstream depin { dep_filename };
  string line;
  bool found_target = false;

  while ( getline( depin, line ) ) {
    if ( line.length() < 2 ) {
      if ( found_target ) { break; }
      else { continue; }
    }

    if ( not found_target and
         line.compare( 0, target_str.size(), target_str ) == 0 ) {
      line = line.substr( line.find( ':' ) + 1, line.length() );
      found_target = true;
    }
    else if ( found_target and
              line.find( ":" ) != string::npos ) {
      break;
    }

    if ( not found_target ) {
      continue;
    }

    line = line.substr( 1, line.length() );

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

const bool FAST_DEPS = ( getenv( "GG_FAST_DEPS") != nullptr );

vector<string> GCCModelGenerator::generate_dependencies_file( const InputFile & input,
                                                              const vector<string> & option_args,
                                                              const string & output_name,
                                                              const string & target_name )
{
  const string & input_filename = input.name;

  vector<string> args;
  args.reserve( 1 + option_args.size() );

  if ( operation_mode_ == OperationMode::GCC ) {
    args.push_back( "gcc-7" );
  }
  else {
    args.push_back( "g++-7" );
  }

  if ( getenv( "GG_REMODELING" ) != nullptr ) {
    roost::create_directories( roost::dirname( output_name ) );
  }

  args.insert( args.end(), option_args.begin(), option_args.end() );

  string output_filename;

  const bool has_dependencies_option = find_if(
    args.begin(), args.end(),
    []( const string & opt )
    {
      return ( opt == "-M" ) or ( opt == "-MF" ) or ( opt == "-MM" ) or
             ( opt == "-MG" ) or ( opt == "-MP" ) or ( opt == "-MQ" ) or
             ( opt == "-MD" ) or ( opt == "-MMD" );
    } ) != end( args );

  if ( has_dependencies_option ) {
    auto m_search = find( args.begin(), args.end(), "-M" );
    auto mf_search = find( args.begin(), args.end(), "-MF" );
    auto md_search = find( args.begin(), args.end(), "-MD" );

    if ( mf_search == end( args ) ) {
      throw runtime_error( "cannot produce dependencies file without -MF option" );
    }

    if ( md_search != end( args ) ) {
      args.erase( md_search );

      if ( m_search != end( args ) ) {
        args.push_back( "-M" );
      }
    }
  }
  else {
    args.push_back( "-M" );
    args.push_back( "-MT" );
    args.push_back( target_name );
  }

  /* do we have a valid cache for these dependencies? */
  const string input_file_hash = gg::hash::file( input_filename );
  const auto cache_entry_path = gg::paths::dependency_cache_entry( input_file_hash );

  /* assemble the function */
  const Function makedep_fn { args.front(), args, gcc_environment() };

  if ( roost::exists( cache_entry_path ) ) {
    /* abuse the thunk format to store a cache of dependencies */
    const Thunk dep_cache_entry = ThunkReader::read( cache_entry_path );

    /* do we have a possible cache hit? */
    if ( makedep_fn == dep_cache_entry.function() ) {
      bool cache_hit = true;

      /* check if all the infiles are still the same */
      for ( const auto & item : dep_cache_entry.values() ) {
        if ( not Thunk::matches_filesystem( item ) ) {
          cache_hit = false;
          break;
        }
      }

      if ( cache_hit ) {
        return write_dependencies_file( output_name, target_name, dep_cache_entry.values() );
      }
    }
  }

  if ( not has_dependencies_option ) {
    args.push_back( "-MF" );
    args.push_back( output_name );
  }

  /* if ( FAST_DEPS ) {
    Optional<vector<string>> fast_infiles_list;

    try {
      fast_infiles_list.reset( move( scan_dependencies( input_filename, input.language ) ) );

      if ( has_dependencies_option ) {
        write_dependencies_file( output_name, target_name, *fast_infiles_list );
      }

      infiles_list = move( *fast_infiles_list );
      fast_deps_successful = true;
    }
    catch ( const CouldNotParse & ex ) {
      cerr << "[error]" << ex.what() << endl;
    }
  } */

  // (1) do it the fast way
  Optional<vector<string>> fast_infiles_list;
  fast_infiles_list.reset( move( scan_dependencies( input_filename, input.language ) ) );

  // (2) do it the slow way
  vector<string> infiles_list;
  run( args[ 0 ], args, {}, true, true );
  infiles_list = parse_dependencies_file( output_name, target_name );

  if ( not has_dependencies_option ) {
    args.pop_back();
    args.pop_back();
  }

  /* write a cache entry for next time */

  /* assemble the infiles */

  /* let's compare this list and the fast list */
  if ( fast_infiles_list.initialized() ) {
    bool failed = false;
    for ( const string & file : infiles_list ) {
      if ( find( fast_infiles_list->begin(), fast_infiles_list->end(), file ) == fast_infiles_list->end() ) {
        failed = true;
        cerr << "could not find " << file << " in fast infiles list." << endl;
      }
    }

    if ( failed ) {
      for ( const string & found_files : *fast_infiles_list ) {
        cerr << "* " << found_files << endl;
      }
      throw runtime_error( "^^" );
    }

    cerr << "[info] found everything for '" << input_filename << "'" << endl;
  }

  vector<Thunk::DataItem> dependencies;
  for ( const auto & str : infiles_list ) {
    dependencies.emplace_back( make_pair( gg::hash::file( str ), str ) );
  }

  Thunk dep_cache_entry( makedep_fn, dependencies,
                         { make_pair( makedep_fn.hash(), "" ) },
                         { "fake_output" } );

  /* serialize and write the fake thunk */
  string serialized_cache_entry { ThunkWriter::serialize( dep_cache_entry ) };
  roost::atomic_create( serialized_cache_entry, cache_entry_path );
  cerr << "Creating dependency cache entry.\n";

  return write_dependencies_file( output_name, target_name, dep_cache_entry.values() );
}

/* this is a fast (but imperfect) dependency scanner that is
   faster than gcc -M */
/* the goal is to bias in favor of false positives (header files that
   aren't actually used */
/* this way the thunk will at least contain all the necessary files
   needed at compile time */
/* we need to emulate gcc's search behavior exactly to make sure we
   find the *right* path for each include statement */

Optional<pair<roost::path, size_t>> find_file_in_path_list( const roost::path & parent_filename,
                                                            const roost::path & filename,
                                                            const vector<roost::path> & search_path,
                                                            size_t start_index )
{
  /* cerr << "searching for file " << filename.string() << " in directory path ";
  for ( size_t i = start_index; i < search_path.size(); i++ ) {
    cerr << search_path[ i ].string() << ":";
  } */

  if ( start_index == 0 ) {
    start_index++;

    roost::path parent_dir = roost::dirname( parent_filename );
    const roost::path candidate = ( parent_dir.string() == "." ) ? filename
                                                                 : ( parent_dir / filename );

    if ( roost::exists( candidate ) ) {
      return { true, make_pair( candidate, 0 ) };
    }
  }

  for ( size_t i = start_index; i < search_path.size(); i++ ) {
    const auto candidate = search_path[ i ] / filename;
    if ( roost::exists( candidate ) ) {
      // cerr << "-> found " << candidate.string() << "\n";
      return { true, make_pair( candidate, i ) };
    }
  }

  // cerr << "-> not found\n";
  return {};
}

/* remove repetitive './'s from the beginning of the path names */
string simple_path_trim( const string & path ) {
  string res = path;
  while ( res.compare( 0, 2, "./" ) == 0 ) {
    res.erase( 0, 2 );
  }
  return res;
}

void GCCModelGenerator::scan_dependencies_recursive( const roost::path & filename,
                                                     unordered_set<string> & dependencies,
                                                     const Language source_language,
                                                     const vector<roost::path> & include_path,
                                                     const size_t current_include_path_index )
{
  // cerr << "scan_dependencies_recursive called on " << filename.string() << "\n";

  const string trimmed_path = simple_path_trim( filename.string() );

  /* base case: if we've already looked at this file, ignore */
  if ( dependencies.count( trimmed_path ) ) {
    return;
  }

  /* otherwise, recursive case: add this file as a dependency, and recurse */
  dependencies.insert( trimmed_path );

  /* now recurse */

  /* read the file */
  const string file_data = roost::read_file( filename );

  /* scan for #include or #include_next */
  size_t index = 0;
  while ( index < file_data.size() ) {
    //    cerr << "index = " << index << "\n";
    /* search for # */
    const size_t hash_index = file_data.find( '#', index );
    if ( hash_index == string::npos ) {
      break;
    }

    if ( hash_index > 0 and not isspace( file_data[ hash_index - 1 ] ) and
         file_data[ hash_index - 1 ] != '/' ) {
      /* this is not the # we're looking for. */
      /* this line is mainly added to protect again lines like this:
         "This is a Standard C++ Library file.  You should @c \#include this file"
         that can be found in a lot of system libraries */
      index = hash_index + 1;
      continue;
    }

    //    cerr << "hash_index = " << hash_index << "\n";

    /* search past whitespace */
    const size_t directive_index = file_data.find_first_not_of( " \t", hash_index + 1 );
    if ( directive_index == string::npos ) {
      break;
    }

    //    cerr << "directive_index = " << directive_index << "\n";

    bool is_include_next = false;
    constexpr const char str_include_next[] = "include_next";
    constexpr const char str_include[] = "include";

    /* is it an include_next? */
    constexpr size_t len_include_next = sizeof( str_include_next ) - 1;
    constexpr size_t len_include = sizeof( str_include ) - 1;
    if ( file_data.compare( directive_index, len_include_next, str_include_next ) ) {
      /* it's not include next, could it be include? */
      if ( file_data.compare( directive_index, len_include, str_include ) ) {
        /* skip ahead */
        index = directive_index + 1;
        continue;
      }

      /* it's include! */
      is_include_next = false;
    }
    else {
      /* it's include_next */
      is_include_next = true;
    }

    // cerr << "found include at position " << directive_index << "\n";

    index = directive_index + ( is_include_next ? len_include_next : len_include );

    /* we found an include statement */
    /* search past whitespace again */
    const size_t bracketed_filename_index = file_data.find_first_not_of( " \t", index );
    if ( bracketed_filename_index == string::npos ) {
      throw CouldNotParse( "include prefix without filename" );
    }

    // cerr << "found bracketed include filename at position " << bracketed_filename_index << "\n";

    const char open_bracket = file_data.at( bracketed_filename_index );
    char closing_bracket;
    if ( open_bracket == '<' ) {
      closing_bracket = '>';
    } else if ( open_bracket == '"' ) {
      closing_bracket = '"';
    } else {
      /* okay, maybe this #include or #include next is inside a comment section! */
      const size_t prev_open  = file_data.rfind( "/*", hash_index );
      const size_t prev_close = file_data.rfind( "*/", hash_index );
      const size_t next_open  = file_data.find(  "/*", index + 1 );
      const size_t next_close = file_data.find(  "*/", index + 1 );

      bool commented = ( prev_open != string::npos and
                         ( prev_close < prev_open or prev_close == string::npos ) ) and
                       ( next_close != string::npos ) and
                         ( next_close < next_open );

      if ( commented ) {
        index = next_close + 2;
        continue;
      }

      throw CouldNotParse( "unexpected bracket character in '" + filename.string() + "'" );
    }

    /* find closing bracket */
    const size_t closing_bracket_index = file_data.find( closing_bracket, bracketed_filename_index + 1 );
    if ( closing_bracket_index == string::npos ) {
      throw CouldNotParse( "missing closing bracket" );
    }

    /* extract filename */
    const string included_filename = file_data.substr( bracketed_filename_index + 1, closing_bracket_index - bracketed_filename_index - 1 );
    // cerr << "found included filename: {" << included_filename << "}\n";

    /* step 2a: search for the file in the include path */
    const size_t search_start_index = max( ( open_bracket == '"' ) ? 0ul : 1ul,
                                           is_include_next ? ( current_include_path_index + 1 ) : 0u );

    const auto full_filename = find_file_in_path_list( filename,
                                                       included_filename,
                                                       include_path,
                                                       search_start_index );

    /* step 2: recurse to scan this resolved pathname */
    if ( full_filename.initialized() ) {
      scan_dependencies_recursive( full_filename->first, dependencies, source_language,
                                   include_path, full_filename->second );
    }
  }
}

vector<string> GCCModelGenerator::scan_dependencies( const roost::path & filename,
                                                     const Language source_language )
{
  /* step 1: assemble the include path */
  vector<roost::path> include_path;

  /* lookup order from `info gcc 'Directory Options'` */

  /* 1: current directory for #include "file" */
  include_path.emplace_back( "" );

  /* 2: skip -iquote */

  /* 3: -I */
  include_path.insert( include_path.end(),
                       arguments_.include_dirs().begin(),
                       arguments_.include_dirs().end() );

  /* 4: -isystem */
  include_path.insert( include_path.end(),
                       arguments_.system_include_dirs().begin(),
                       arguments_.system_include_dirs().end() );

  /* 5: standard system directories */
  if ( arguments_.no_stdinc() ) {
    /* do nothing */
  } else if ( arguments_.no_stdincpp() ) {
    include_path.insert( include_path.end(),
                         c_include_path.begin(),
                         c_include_path.end() );
  } else if ( source_language == Language::C or
              source_language == Language::C_HEADER or
              source_language == Language::ASSEMBLER_WITH_CPP ) {
    include_path.insert( include_path.end(),
                         c_include_path.begin(),
                         c_include_path.end() );
  } else {
    include_path.insert( include_path.end(),
                         cxx_include_path.begin(),
                         cxx_include_path.end() );
  }

  /* 6: skip -idirafter */

  /* recursively find the dependencies for this file */
  unordered_set<string> dependencies;
  scan_dependencies_recursive( filename, dependencies, source_language,
                               include_path, 0 );

  /* XXX cache the results */

  return { dependencies.begin(), dependencies.end() };
}
