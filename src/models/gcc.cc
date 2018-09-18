/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "gcc.hh"

#include <vector>
#include <string>
#include <tuple>
#include <algorithm>
#include <map>
#include <array>
#include <memory>
#include <fstream>
#include <sstream>
#include <cmath>
#include <chrono>
#include <getopt.h>
#include <libgen.h>
#include <sys/ioctl.h>

#include "timeouts.hh"
#include "protobufs/util.hh"
#include "thunk/factory.hh"
#include "thunk/placeholder.hh"
#include "thunk/ggutils.hh"
#include "thunk/thunk.hh"
#include "util/digest.hh"
#include "util/exception.hh"
#include "util/iterator.hh"
#include "util/optional.hh"
#include "util/system_runner.hh"
#include "util/temp_file.hh"
#include "util/tokenize.hh"
#include "util/timeit.hh"
#include "util/util.hh"
#include "util/ipc_socket.hh"

using namespace std;
using namespace std::chrono;
using namespace gg::thunk;

class Blueprints
{
private:
  vector<pair<string, string>> items_;

public:
  Blueprints();
  string get( const roost::path & path ) const;
};

Blueprints::Blueprints()
  : items_()
{
  roost::path blueprints_index = gg::paths::blueprints() / "index";
  if ( not roost::exists( blueprints_index ) ) {
    return;
  }

  ifstream fin { blueprints_index.string() };
  string line;

  while ( getline( fin, line ) ) {
    if ( line.length() < gg::hash::length + 2 ) {
      throw runtime_error( "invalid blueprints index format" );
    }

    const string hash = line.substr( 0, gg::hash::length );
    string path = line.substr( gg::hash::length + 1 );
    if ( path.back() != '/' ) { path.append( 1, '/' ); }
    items_.emplace_back( path, hash );
  }
}

string Blueprints::get( const roost::path & path ) const
{
  string path_str = ( path / "" ).string();
  for ( const auto & item : items_ ) {
    if ( path_str.compare( 0, item.first.length(), item.first ) == 0 ) {
      return item.second;
    }
  }

  throw runtime_error( "could not find a blueprint for " + path.string() );
}

void dump_gcc_specs( TempFile & target_file )
{
  target_file.write( run( "gcc-7", { "gcc-7", "-dumpspecs" },
                                   {}, true, true, true ) );
}

vector<string> prune_makedep_flags( const vector<string> & args )
{
  vector<string> result;

  for ( auto it = args.begin(); it != args.end(); it++ ) {
    if ( ( *it == "-M" ) or ( *it == "-MD" ) or ( *it == "-MP" ) ) {
      continue;
    }
    else if ( ( *it == "-MF" ) or ( *it == "-MT" ) ) {
      it++;
    }
    else {
      result.push_back( *it );
    }
  }

  return result;
}

bool is_non_object_input( const InputFile & input )
{
  switch( input.language ) {
  case Language::SHARED_LIBRARY:
  case Language::ARCHIVE_LIBRARY:
  case Language::OBJECT:
  case Language::SHARED_OBJECT:
    return false;

  default: return true;
  }
}

bool has_include_or_incbin( const string & filename )
{
  ifstream fin { filename };
  string line;

  while ( getline( fin, line ) ) {
    if ( line.find( ".include" ) != string::npos or
         line.find( ".incbin" ) != string::npos ) {
      return true;
    }
  }

  return false;
}

string GCCModelGenerator::do_preprocessing( const InputFile & input )
{
  vector<string> args { arguments_.all() };

  for ( auto it = arguments_.input_files().rbegin(); it != arguments_.input_files().rend(); it++ ) {
    if ( it->index != input.index ) {
      args.erase( args.begin() + it->index );
    }
    else {
      args.at( input.index ) = input.name;
      args.insert( args.begin() + input.index, language_to_name( input.language ) );
      args.insert( args.begin() + input.index, "-x" );
    }
  }

  args.erase(
    remove_if(
      args.begin(), args.end(),
      []( const string & s ) { return ( s == "-E" or s == "-S" or s == "-c" ); }
    ), args.end()
  );

  args.insert( args.begin(), ( operation_mode_ == OperationMode::GCC ) ? "gcc-7" : "g++-7" );
  args.push_back( "-E" );

  args.push_back( "-frandom-seed=" + BEGIN_REPLACE
                  + input.name
                  + END_REPLACE );

  args.push_back( "-Wno-builtin-macro-redefined" );
  args.push_back( "-D__TIMESTAMP__=\"REDACTED\"" );
  args.push_back( "-D__DATE__=\"REDACTED\"" );
  args.push_back( "-D__TIME__=\"REDACTED\"" );
  args.push_back( "-fno-canonical-system-headers" );

  const auto & temp_output_path_base = gg::paths::blob( roost::rbasename( input.name ).string() );
  UniqueFile temp_output { temp_output_path_base.string() };
  temp_output.fd().close();

  args.push_back( "-o" );
  args.push_back( temp_output.name() );

  run( args[ 0 ], args, {}, true, true );

  const string hash = gg::hash::file( temp_output.name() );
  const auto & actual_path = gg::paths::blob( hash );
  roost::move_file( temp_output.name(), actual_path );

  return hash;
}

bool ends_with( const string & str, const string & ending )
{
    return (str.length() >= ending.length() )
           ? ( 0 == str.compare( str.length() - ending.length(), ending.length(), ending ) )
           : false;
}

void get_all_files( vector<roost::path> & file_list,
                    const roost::path & root,
                    const vector<string> & filters,
                    const vector<roost::path> & excludes )
{
  for ( const auto & entry : roost::get_directory_listing( root ) ) {
    const roost::path entry_path = root / entry;
    if ( roost::exists_and_is_directory( entry_path ) ) {
      bool excluded = false;
      for ( const auto & excluded_path : excludes ) {
        if ( excluded_path.string() == entry_path.string() ) {
          excluded = true;
          break;
        }
      }

      if ( excluded) {
        continue;
      }

      get_all_files( file_list, entry_path, filters, excludes );
      continue;
    }

    for ( const auto & filter : filters ) {
      if ( ends_with( entry_path.string(), filter ) ) {
        file_list.push_back( entry_path );
        break;
      }
    }
  }
}

vector<roost::path> GCCModelGenerator::scan_build_directory( const roost::path & build_dir,
                                                             const vector<roost::path> & excludes ) const
{
  vector<roost::path> result;
  vector<string> filters;

  ifstream fin { ( build_dir / "headers.gg.txt" ).string() };
  if ( not fin.good() ) {
    throw runtime_error( "cannot open headers.gg.txt" );
  }

  string line;

  while ( getline( fin, line ) ) {
    filters.push_back( line );
  }

  get_all_files( result, build_dir, filters, excludes );
  return result;
}

string GCCModelGenerator::generate_thunk( const GCCStage first_stage,
                                          const GCCStage stage,
                                          const InputFile & input,
                                          const string & output,
                                          const bool write_placeholder )
{
  auto & gcc_data = program_data.at( ( operation_mode_ == OperationMode::GCC ) ? GCC : GXX );

  vector<string> args { arguments_.all() };

  /* there can be other inputs in the args, we have to remove them first */
  for ( auto it = arguments_.input_files().rbegin(); it != arguments_.input_files().rend(); it++ ) {
    if ( it->index != input.index ) {
      args.erase( args.begin() + it->index );
    }
    else {
      args.at( input.index ) = input.name;
      args.insert( args.begin() + input.index, language_to_name( input.language ) );
      args.insert( args.begin() + input.index, "-x" );
    }
  }
  /* now 'args' only has the input file we are generating the thunk for */

  args.erase(
    remove_if(
      args.begin(), args.end(),
      []( const string & s ) { return ( s == "-E" or s == "-S" or s == "-c" ); }
    ), args.end()
  );

  /* Common infiles */
  vector<ThunkFactory::Data> base_infiles = { input.indata };
  vector<ThunkFactory::Data> base_executables = { gcc_data };

  base_infiles.emplace_back( "/__gg__/gcc-specs", specs_tempfile_.name() );

  for ( const string & extra_infile : arguments_.extra_infiles( stage ) ) {
    base_infiles.emplace_back( extra_infile );
  }

  /* Common dummy directories */
  vector<string> dummy_dirs;

  if ( stage != PREPROCESS ) {
    /* For preprocess stage, we are going to use `args` to get the
       dependencies, so we can't have this FAKE specs path here.
       We will add these later.*/
    args.insert( args.begin(), { gcc_data.filename(), "-specs=/__gg__/gcc-specs" } );
    args.push_back( "-o" );
    args.push_back( output );
  }

  bool generate_makedep_file = find_if( args.begin(), args.end(),
      [] ( const string & dep ) {
        return ( dep == "-M" ) or ( dep == "-MM" ) or ( dep == "-MD" );
      }
    ) != end( args );

  string generated_thunk_hash;

  switch ( stage ) {
  case PREPROCESS:
  {
    const vector<string> & include_path =
      ( input.language == Language::C or
        input.language == Language::C_HEADER or
        input.language == Language::ASSEMBLER_WITH_CPP )
        ? c_include_path
        : cxx_include_path;

     /* ARGS */
    args.push_back( "-E" );

    args.push_back( "-frandom-seed=" + BEGIN_REPLACE
                    + input.name
                    + END_REPLACE );

    args.push_back( "-Wno-builtin-macro-redefined" );
    args.push_back( "-D__TIMESTAMP__=\"REDACTED\"" );
    args.push_back( "-D__DATE__=\"REDACTED\"" );
    args.push_back( "-D__TIME__=\"REDACTED\"" );
    args.push_back( "-fno-canonical-system-headers" );

    vector<string> all_args;
    all_args.reserve( c_include_path.size() + args.size() + 2 );
    all_args.insert( all_args.end(), args.begin(), args.end() );

    if ( arguments_.no_stdinc() ) {
      /* do nothing */
    }
    else if ( arguments_.no_stdincpp() ) {
      if ( input.language == Language::CXX or
           input.language == Language::CXX_HEADER ) {
        all_args.push_back( "-nostdinc" );
        for ( const auto & p : c_include_path ) {
          all_args.push_back( "-isystem" + p );
        }
      }
    }
    else {
      all_args.push_back( "-nostdinc" );

      for ( const auto & p : include_path ) {
        all_args.push_back( "-isystem" + p );
      }
    }

    /* Generate dependency list */
    vector<string> dependencies;

    TempFile makedep_tempfile { "/tmp/gg-makedep" };
    string makedep_filename;
    string makedep_target = DEFAULT_MAKE_TARGET;

    if ( generate_makedep_file ) {
      makedep_filename = *arguments_.option_argument( GCCOption::MF );
      Optional<string> mt_arg = arguments_.option_argument( GCCOption::MT );
      if ( mt_arg.initialized() ) {
        makedep_target = *mt_arg;
      }
      else {
        makedep_target = roost::rbasename( input.name ).string();
        string::size_type dot_pos = makedep_target.rfind( '.' );
        if ( dot_pos != string::npos ) {
            makedep_target = makedep_target.substr( 0, dot_pos );
        }
        makedep_target += ".o";
      }
    }
    else {
      makedep_filename = makedep_tempfile.name();
    }

    if ( not defer_depgen_ ) {
      cerr << "\u251c\u2500 generating make dependencies file... ";
      dependencies = generate_dependencies_file( operation_mode_, input.name,
                                                 all_args, makedep_filename,
                                                 makedep_target );
      cerr << "done." << endl;
    }

    /* We promised that we would add these here, and we lived up to our
       promise... */
    all_args.insert( all_args.begin(), { gcc_data.filename(), "-specs=/__gg__/gcc-specs" } );
    all_args.push_back( "-o" );
    all_args.push_back( output );
    all_args = prune_makedep_flags( all_args );

    /* INFILES */
    string cc1_program = CC1;

    if ( input.language == Language::C or
         input.language == Language::C_HEADER or
         input.language == Language::ASSEMBLER_WITH_CPP ) {
      base_executables.emplace_back( program_data.at( CC1 ) );
    }
    else {
      base_executables.emplace_back( program_data.at( CC1PLUS ) );
      cc1_program = CC1PLUS;
    }

    for ( const string & dep : dependencies ) {
      base_infiles.emplace_back( dep );
    }

    for ( const string & dir : include_path ) {
      dummy_dirs.push_back( dir );
    }

    for ( const string & dir : arguments_.include_dirs() ) {
      dummy_dirs.push_back( dir );
    }

    for ( const string & dir : arguments_.system_include_dirs() ) {
      dummy_dirs.push_back( dir );
    }

    dummy_dirs.push_back( "." );

    vector<ThunkFactory::Output> thunk_outputs { { "output", output } };
    vector<string> preprocess_envars = envars_;
    string depgen_binary;
    string depgen_hash;

    /* stuff related to deferred dependency generation -------------*/
    if ( defer_depgen_ ) {
      auto create_envar = [] ( const string & name, const string & value ) {
        return name + '=' + value;
      };

      Optional<roost::path> build_dir;
      if ( getenv( "GG_GCC_BUILD_DIR" ) != nullptr ) {
        build_dir.reset( roost::canonical( safe_getenv( "GG_GCC_BUILD_DIR" ) ) / "" );
      }

      /* path that are excluded from the search */
      vector<roost::path> excludes { gg::paths::root() };
      if ( getenv( "GG_GCC_SOURCE_DIR" ) != nullptr ) {
        excludes.emplace_back( safe_getenv( "GG_GCC_SOURCE_DIR" ) );
      }

      /* (0) let's make sure that we have blueprints for everything first */
      Blueprints blueprints;
      unordered_set<string> tarballs;

      auto process_directory_blueprints =
        [&] ( const string & dir )
        {
          if ( not roost::exists( dir ) ) {
            return; /* this directory doesn't even exist */
          }

          const roost::path canonical_dir = roost::canonical( dir ) / "";

          /* if build dir is set, don't look for a blueprint -- it will be
             handled separately */
          if ( build_dir.initialized() and
               canonical_dir.string().compare( 0, build_dir->string().length(),
                                                  build_dir->string() ) == 0 ) {
            return;
          }

          const string hash = blueprints.get( canonical_dir );
          const roost::path src = gg::paths::blueprint( hash );
          const roost::path dst = gg::paths::blob( hash );

          if ( not roost::exists( dst ) ) {
            roost::copy_then_rename( src, dst, true, 0400 );
          }

          tarballs.insert( hash );
        };

      for ( const string & dir : arguments_.include_dirs() ) {
        process_directory_blueprints( dir );
      }

      for ( const string & dir : arguments_.system_include_dirs() ) {
        process_directory_blueprints( dir );
      }

      if ( not arguments_.no_stdinc() ) {
        for ( const string & dir : ( arguments_.no_stdincpp() ? c_include_path
                                                              : include_path ) ) {
          process_directory_blueprints( dir );
        }
      }

      string include_tarballs_str;
      for ( const auto & tarball : tarballs ) {
        if ( include_tarballs_str.empty() ) { include_tarballs_str += tarball; }
        else { include_tarballs_str += ":" + tarball; }
      }

      /* (1) setting the necessary environment variables */
      preprocess_envars.emplace_back(
        create_envar( DEPGEN_WORKING_DIRECTORY, roost::current_working_directory().string() ) );
      preprocess_envars.emplace_back(
        create_envar( DEPGEN_INCLUDE_TARBALLS, include_tarballs_str ) );
      preprocess_envars.emplace_back(
        create_envar( DEPGEN_INPUT_NAME, input.name ) );
      preprocess_envars.emplace_back(
        create_envar( DEPGEN_TARGET_NAME, makedep_target ) );
      preprocess_envars.emplace_back(
        create_envar( DEPGEN_GCC_HASH, gcc_data.hash() ) );
      preprocess_envars.emplace_back(
        create_envar( DEPGEN_CC1_NAME, cc1_program ) );
      preprocess_envars.emplace_back(
        create_envar( DEPGEN_CC1_HASH, program_data.at( cc1_program ).hash() ) );

      /* (2) add the tarballs to the values */
      for ( const auto & tarball : tarballs ) {
        base_infiles.emplace_back( ".", "", gg::ObjectType::Value, tarball );
      }

      /* (3) add the header files in build directory to thunk */
      vector<roost::path> files;

      if ( build_dir.initialized() ) {
        files = scan_build_directory( *build_dir, excludes );
        for ( const auto & file : files ) {
          base_infiles.emplace_back( file.string() );
        }
      }

      thunk_outputs.emplace_back( "dependencies" );

      all_args[ 0 ] = "/__gg__/generate-deps";

      depgen_binary = safe_getenv( "GG_GCC_DEPGEN_BIN" );
      depgen_hash = gg::hash::file( depgen_binary );
      base_executables.emplace_back( "/__gg__/generate-deps",
                                     depgen_binary,
                                     gg::ObjectType::Value,
                                     depgen_hash );
    }

    Function function = ( defer_depgen_ )
                      ? Function { depgen_hash, all_args, preprocess_envars }
                      : gcc_function( operation_mode_, all_args, preprocess_envars );
    /*-------------------------------------------------------------*/

    generated_thunk_hash = ThunkFactory::generate(
      function,
      base_infiles,
      base_executables,
      thunk_outputs,
      dummy_dirs,
      ( defer_depgen_ ) ? DEPGEN_TIMEOUT : PREPROCESS_TIMEOUT,
      ThunkFactory::Options::collect_data
        | ThunkFactory::Options::generate_manifest
        | ThunkFactory::Options::include_filenames
    );

    if ( defer_depgen_ and generate_makedep_file ) {
      /* let's create a placeholder for the dependencies file */
      ofstream fout_dep { makedep_filename };
      fout_dep << makedep_target << ": " << input.name << endl;
      // ThunkPlaceholder deps_placeholder( gg::hash::for_output( generated_thunk_hash, "dependencies" ) );
      // deps_placeholder.write( makedep_filename );
    }

    break;
  }

  /******************************************************************/

  case COMPILE:
    args.push_back( "-S" );

    args.push_back( "-frandom-seed=__GG_BEGIN_REPLACE__"
                    + input.name
                    + "__GG_END_REPLACE__" );

    args = prune_makedep_flags( args );

    if ( input.language == Language::CPP_OUTPUT ) {
      base_executables.push_back( program_data.at( CC1 ) );
    }
    else {
      base_executables.push_back( program_data.at( CC1PLUS ) );
    }

    generated_thunk_hash = ThunkFactory::generate(
      gcc_function( operation_mode_, args, envars_ ),
      base_infiles,
      base_executables,
      { { "output", output } },
      dummy_dirs,
      COMPILE_TIMEOUT,
      ThunkFactory::Options::collect_data
        | ThunkFactory::Options::generate_manifest
        | ThunkFactory::Options::include_filenames
    );

    break;

  /******************************************************************/

  case ASSEMBLE:
    if ( first_stage != ASSEMBLE and
         ( arguments_.option_argument( GCCOption::gdwarf_4 ).initialized() or
           arguments_.option_argument( GCCOption::g ).initialized() or
           arguments_.option_argument( GCCOption::g0 ).initialized() or
           arguments_.option_argument( GCCOption::g1 ).initialized() ) ) {
      args.erase(
        remove_if(
          args.begin(), args.end(),
          []( const string & s ) { return ( s == "-gdwarf-4" or s == "-g" or s == "-g0" or s == "-g1" ); }
        ), args.end()
      );
    }

    args.push_back( "-c" );
    args = prune_makedep_flags( args );
    base_executables.push_back( program_data.at( AS ) );

    generated_thunk_hash = ThunkFactory::generate(
      gcc_function( operation_mode_, args, envars_ ),
      base_infiles,
      base_executables,
      { { "output", output } },
      dummy_dirs,
      ASSEMBLE_TIMEOUT,
      ThunkFactory::Options::collect_data
        | ThunkFactory::Options::generate_manifest
        | ThunkFactory::Options::include_filenames
    );

    break;

  /******************************************************************/

  default: throw runtime_error( "invalid gcc stage" );
  }

  if ( write_placeholder ) {
    ThunkPlaceholder placeholder { generated_thunk_hash };
    placeholder.write( output );
  }

  return generated_thunk_hash;
}

void print_gcc_command( const string & command_str )
{
  struct winsize size;
  ioctl( STDOUT_FILENO, TIOCGWINSZ, &size );
  size_t window_width = size.ws_col - 5;

  cerr << "\u2503  ";

  size_t line_count = static_cast<size_t>( ceil( 1.0 * command_str.length() / window_width ) );

  for ( size_t i = 0; i < line_count; i++ ) {
    if ( i > 0 ) {
      cerr << " \u2936" << endl;
      cerr << "\u2503  ";
    }
   cerr << command_str.substr( i * window_width, window_width );
  }

   cerr << endl;
}

const bool force_strip = ( getenv( "GG_GCC_FORCE_STRIP" ) != nullptr );

GCCModelGenerator::GCCModelGenerator( const OperationMode operation_mode,
                                      int argc, char ** argv, const int options )
  : operation_mode_( operation_mode ),
    arguments_( argc, argv, force_strip ),
    preprocess_locally_( options & Options::preprocess_locally ),
    all_in_one_thunk_( options & Options::all_in_one_thunk ),
    defer_depgen_( options & Options::defer_depgen )
{
  exec_original_gcc = [&argv]() { _exit( execvp( argv[ 0 ], argv ) ); };

  if ( getenv( "GG_BYPASS" ) != nullptr ) {
    cerr << "\u2570\u257c bypassing model generation, executing gcc..." << endl;
    exec_original_gcc();
  }

  if ( arguments_.option_argument( GCCOption::print_file_name ).initialized() or
       arguments_.option_argument( GCCOption::dM ).initialized() or
       arguments_.option_argument( GCCOption::dumpmachine ).initialized()  ) {
    // just run gcc for this
    cerr << "\u2570\u257c special option is present, executing gcc..." << endl;
    exec_original_gcc();
  }

  for ( const InputFile & input : arguments_.input_files() ) {
    if ( input.source_language == Language::ASSEMBLER_WITH_CPP and
         has_include_or_incbin( input.name ) ) {
      cerr << "\u2570\u257c assembler file has .include or .incbin directive, executing gcc..." << endl;
      exec_original_gcc();
    }
  }

  if ( arguments_.last_stage() == PREPROCESS and
       ( arguments_.output_filename() == "-" or
         arguments_.output_filename().empty() ) ) {
    // preprocessing to the standard output, just do it, don't create a thunk.

    cerr << "\u2570\u257c output is stdout, executing gcc..." << endl;
    exec_original_gcc();
  }
  else if ( arguments_.output_filename() == "-" ) {
    throw runtime_error( "outputting to stdout is only allowed for preprocessing stage" );
  }

  if ( arguments_.input_files().size() == 1 and arguments_.input_files()[ 0 ].name == "/dev/null" ) {
    // just run gcc
    cerr << "\u2570\u257c input is /dev/null, executing gcc..." << endl;
    exec_original_gcc();
  }

  if ( arguments_.input_files().size() == 0 ) {
    throw runtime_error( "no input files" );
  }

  dump_gcc_specs( specs_tempfile_ );
}

void GCCModelGenerator::generate()
{
  string final_output = arguments_.output_filename();
  GCCStage last_stage = arguments_.last_stage();
  vector<InputFile> input_files = arguments_.input_files();

  /* count non-object input files */
  const size_t source_inputs = count_if( input_files.begin(),
                                         input_files.end(),
                                         []( const InputFile & input ) {
                                           return is_non_object_input( input );
                                         } );

  if ( source_inputs > 1 and not final_output.empty() ) {
    throw runtime_error( "cannot specify -o with -c, -S or -E with multiple files" );
  }

  for ( size_t input_index = 0; input_index < input_files.size(); input_index++ ) {
    InputFile & input = input_files.at( input_index );

    if ( input.language != Language::SHARED_LIBRARY ) {
      input.indata = move( ThunkFactory::Data( input.name ) );
    }

    if ( not is_non_object_input( input ) ) {
      continue;
    }

    const GCCStage first_stage = language_to_stage( input.language );
    const GCCStage input_last_stage = ( last_stage == LINK ) ? ASSEMBLE : last_stage;

    map<size_t, string> stage_output;
    stage_output[ first_stage - 1 ] = input.name;

    cerr << "\u251c\u257c input: " << input.name << endl;

    for ( size_t stage_num = first_stage; stage_num <= input_last_stage; stage_num++ ) {
      const GCCStage stage = static_cast<GCCStage>( stage_num );
      string output_name;

      if ( input.source_language == Language::ASSEMBLER_WITH_CPP and
           stage == COMPILE ) {
        /* we should skip this stage */
        continue;
      }

      if ( stage == last_stage ) {
        if ( final_output.empty() ) {
          /* fill in default names based on the original input filename,
             e.g. hello.abc.cc -> hello.s in compile stage, hello.abc.o in assembly */
          if ( stage == PREPROCESS ) {
            throw runtime_error( "gcc model cannot preprocess to stdout" );
          } else if ( stage == LINK ) {
            throw runtime_error( "internal error" );
          } else {
            final_output = stage_output_name( stage, split_source_name( input.name ).first );
          }
        }

        output_name = final_output;
      }
      else {
        output_name = "output_" + to_string( input_index ) + "_"
                                + to_string( stage_num );
      }

      string last_stage_hash;
      const bool preprocess_this_locally = preprocess_locally_ and stage == PREPROCESS and stage != last_stage;

      if ( preprocess_this_locally ) {
        last_stage_hash = do_preprocessing( input );
      }
      else {
        last_stage_hash = generate_thunk( first_stage, stage, input, output_name, stage == last_stage );
      }

      switch ( stage ) {
      case PREPROCESS:
        /* generate preprocess thunk */
        cerr << "\u251c\u2500 preprocessed: " << last_stage_hash;
        if ( preprocess_this_locally ) {
          cerr << " (done locally)";
        }
        cerr << endl;

        switch ( input.language ) {
        case Language::C:
        case Language::C_HEADER:
          input.language = Language::CPP_OUTPUT;
          break;

        case Language::CXX_HEADER:
        case Language::CXX:
          input.language = Language::CXX_CPP_OUTPUT;
          break;

        case Language::ASSEMBLER_WITH_CPP:
          input.language = Language::ASSEMBLER;
          break;

        default:
          throw runtime_error( "invalid preprocessing language" );
        }

        break;

      case COMPILE:
        /* generate compile thunk */
        cerr << "\u251c\u2500 compiled: " << last_stage_hash << endl;
        input.language = Language::ASSEMBLER;
        break;

      case ASSEMBLE:
      {
        /* generate assemble thunk */
        cerr << "\u251c\u2500 assembled: " << last_stage_hash << endl;
        input.language = Language::OBJECT;
        break;
      }

      default:
        throw runtime_error( "unexpected stage" );
      }

      input.name = output_name;
      input.indata = ThunkFactory::Data( input.name, "nonexistent",
                                         gg::hash::type( last_stage_hash ),
                                         last_stage_hash );

      if ( stage == last_stage ) {
        cerr << "\u2570\u257c output: " << final_output << endl;
      }
    }
  }

  if ( last_stage == LINK ) {
    if ( final_output.length() == 0 ) {
      final_output = "a.out";
    }

    vector<string> dependencies = get_link_dependencies( input_files );
    string last_stage_hash = generate_link_thunk( input_files, dependencies, final_output );

    cerr << "\u251c\u2500 linked: " << last_stage_hash << endl;
    cerr << "\u2570\u257c output: " << final_output << endl;
  }
}

void usage( const char * arg0 )
{
  cerr << arg0 << " (gcc|g++) [GCC ARGUMENTS]" << endl;
}

int main( int argc, char * argv[] )
{
  try {
    gg::models::init();

    if ( argc <= 0 ) {
      abort();
    }

    if ( argc < 3 ) {
      usage( argv[ 0 ] );
      return EXIT_FAILURE;
    }

    gg::paths::fix_path_envar();

    OperationMode operation_mode;

    if ( strcmp( argv[ 1 ], "gcc" ) == 0 ) {
      operation_mode = OperationMode::GCC;
    }
    else if ( strcmp( argv[ 1 ], "g++" ) == 0 ) {
      operation_mode = OperationMode::GXX;
    }
    else {
      throw runtime_error( "invalid operation mode" );
    }

    argv++;
    argc--;

    cerr << "\u256d\u257c generating model for " << argv[ 0 ] << endl;
    /* print_gcc_command( command_str( argc, argv ) ); */

    int options = 0;

    options |= getenv( "GG_GCC_PREPROCESS_LOCALLY" )
               ? GCCModelGenerator::Options::preprocess_locally : 0;
    options |= getenv( "GG_GCC_ALL_IN_ONE_THUNK" )
               ? GCCModelGenerator::Options::all_in_one_thunk : 0;
    options |= getenv( "GG_GCC_DEFER_DEPGEN" )
               ? GCCModelGenerator::Options::defer_depgen : 0;

    GCCModelGenerator gcc_model_generator { operation_mode, argc, argv, options };
    gcc_model_generator.generate();

    return EXIT_SUCCESS;
  }
  catch ( const exception & e ) {
    print_exception( argv[ 0 ], e );
    return EXIT_FAILURE;
  }
}
