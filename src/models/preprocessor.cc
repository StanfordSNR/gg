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

  vector<string> infiles_list;
  bool fast_deps_successful = false;

  if ( fast_deps_ ) {
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
      cerr << "[fastdeps failed] " << ex.what() << endl;
    }
  }

  if ( not fast_deps_successful ) {
    run( args[ 0 ], args, {}, true, true );
    infiles_list = parse_dependencies_file( output_name, target_name );
  }

  if ( not has_dependencies_option ) {
    args.pop_back();
    args.pop_back();
  }

  /* let's compare this list and the fast list */
  /* if ( fast_infiles_list.initialized() ) {
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
  } */

  /* write a cache entry for next time */

  /* assemble the infiles */
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
  // cerr << "Creating dependency cache entry.\n";

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

    if ( roost::does_exist_and_is_regular_file( candidate ) ) {
      return { true, make_pair( candidate, 0 ) };
    }
  }

  for ( size_t i = start_index; i < search_path.size(); i++ ) {
    const auto candidate = search_path[ i ] / filename;
    if ( roost::does_exist_and_is_regular_file( candidate ) ) {
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
  ifstream fin { trimmed_path };
  string line;

  while ( getline( fin, line ) ) {
    /* only find the preprocessor lines */
    bool is_preprocessor_line = false;
    for ( size_t i = 0; i < line.size(); i++ ) {
      char c = line[ i ];
      if ( isspace( c ) ) { continue; }
      else if ( c == '#' ) { is_preprocessor_line = true; }
      break;
    }

    if ( not is_preprocessor_line ) { continue; }

    const size_t string_start_index = line.find_first_of( "<\"" );
    if ( string_start_index == string::npos ) {
      /* nothing to see here, moving on to the next line! */
      continue;
    }

    const char open_bracket = line[ string_start_index ];
    const char closing_bracket = ( open_bracket == '<' ) ? '>' : '"';

    const size_t string_end_index = line.find( closing_bracket, string_start_index + 1 );
    if ( string_end_index == string::npos ) {
      /* okay, the string was not closed on the same line, let's move on */
      continue;
    }

    /* extract filename */
    const string potential_included_filename = line.substr( string_start_index + 1, string_end_index - string_start_index - 1 );

    /* search for the file in the include path */
    const size_t search_start_index = ( open_bracket == '"' ) ? 0ul : 1ul;
    const size_t include_next_start_index = max( search_start_index, current_include_path_index + 1 );

    const auto full_filename = find_file_in_path_list( filename,
                                                       potential_included_filename,
                                                       include_path,
                                                       search_start_index );

    bool should_do_include_next = ( line.find( "include_next" ) != string::npos );

    /* step 2: recurse to scan this resolved pathname */
    if ( full_filename.initialized() ) {
      scan_dependencies_recursive( full_filename->first, dependencies,
                                   source_language, include_path,
                                   full_filename->second );

      if ( should_do_include_next and search_start_index != include_next_start_index ) {
        const auto full_next_filename = find_file_in_path_list( filename,
                                                                potential_included_filename,
                                                                include_path,
                                                                include_next_start_index );

        if ( full_next_filename.initialized() ) {
          scan_dependencies_recursive( full_next_filename->first,
                                       dependencies, source_language,
                                       include_path,
                                       full_next_filename->second );
        }
      }
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

  /* it's possible some of those defined strings are used as include paths */
  for ( const string & defined_string : arguments_.defined_strings() ) {
    auto result = find_file_in_path_list( filename, defined_string, include_path, 0 );

    if ( result.initialized() ) {
      scan_dependencies_recursive( result->first.string(), dependencies,
                                   source_language, include_path, 0 );
    }
  }

  /* XXX cache the results */

  return { dependencies.begin(), dependencies.end() };
}
