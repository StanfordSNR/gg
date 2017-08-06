/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <vector>
#include <string>
#include <tuple>
#include <algorithm>
#include <map>
#include <array>
#include <memory>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <getopt.h>
#include <libgen.h>

#include "exception.hh"
#include "optional.hh"
#include "digest.hh"
#include "temp_file.hh"
#include "thunk.hh"
#include "utils.hh"

#include "model-gcc.hh"
#include "gcc-args.hh"

using namespace std;
using namespace gg::thunk;

void dump_gcc_specs( TempFile & target_file )
{
  array<char, 4096> buffer;

  std::shared_ptr<FILE> readpipe( popen( "gcc-7 -dumpspecs", "r" ), pclose );

  if ( !readpipe ) {
    throw runtime_error( "could not execute `gcc-7 -dumpspecs`." );
  }

  while ( !feof( readpipe.get() ) ) {
    if ( fgets( buffer.data(), 4096, readpipe.get() ) != nullptr ) {
      string result = buffer.data();
      target_file.write( result );
    }
  }
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
    return false;

  default: return true;
  }
}

Thunk GCCModelGenerator::generate_thunk( const GCCStage stage, const vector<string> & original_args,
                                         const string & input, const string & output )
{
  vector<string> args { original_args };

  args.erase(
    remove_if(
      args.begin(), args.end(),
      []( const string & s ) { return ( s == "-E" or s == "-S" or s == "-c" ); }
    ), args.end()
  );

  bool found_output_flag = false;

  for ( size_t i = 0; i < args.size(); i++ ) {
    if ( args[ i ] == "-o" ) {
      if ( i + 1 == args.size() ) {
        throw runtime_error( "invalid argument: -o option with no argument" );
      }

      args[ ++i ] = output;
      found_output_flag = true;
    }
  }

  vector<InFile> base_infiles = {
    input, program_infiles.at( GCC )
  };

  if ( specs_tempfile_.name().length() ) {
    base_infiles.emplace_back( "/__gg__/gcc-specs", specs_tempfile_.name() );

    if ( stage != PREPROCESS ) {
      /* For preprocess stage, we are going to use `args` to get the
         dependencies, so we can't have this FAKE specs path here. */
      args.insert( args.begin(), "-specs=/__gg__/gcc-specs" );
    }
  }

  if ( not found_output_flag ) {
    args.push_back( "-o" );
    args.push_back( output );
  }

  bool generate_makedep_file = find_if( args.begin(), args.end(),
      [] ( const string & dep ) {
        return ( dep == "-M" ) or ( dep == "-MM" ) or ( dep == "-MD" );
      }
    ) != end( args );

  switch ( stage ) {
  case PREPROCESS:
  {
    TempFile makedep_tempfile { "/tmp/gg-makedep" };
    string makedep_filename;
    string makedep_target = input;

    if ( generate_makedep_file ) {
      cerr << "[+] generating make dependencies file..." << endl;
      generate_dependencies_file( arguments_.option_args(), input, "" );
      makedep_filename = *arguments_.option_argument( GCCOption::MF );
      Optional<string> mt_arg = arguments_.option_argument( GCCOption::MT );
      if ( mt_arg.initialized() ) {
        makedep_target = *mt_arg;
      }
    }
    else {
      generate_dependencies_file( arguments_.option_args(),
                                  input, makedep_tempfile.name() );
      makedep_filename = makedep_tempfile.name();
    }

    vector<string> dependencies = parse_dependencies_file( makedep_filename, makedep_target );

    base_infiles.emplace_back( program_infiles.at( CC1 ) );

    for ( const string & dep : dependencies ) {
      base_infiles.emplace_back( dep );
    }

    for ( const string & dir : c_include_path ) {
      base_infiles.emplace_back( dir, InFile::Type::DUMMY_DIRECTORY );
    }

    for ( const string & dir : arguments_.include_dirs() ) {
      base_infiles.emplace_back( dir, InFile::Type::DUMMY_DIRECTORY );
    }

    base_infiles.emplace_back( ".", InFile::Type::DUMMY_DIRECTORY );

    args.push_back( "-E" );
    args = prune_makedep_flags( args );

    vector<string> all_args;
    all_args.reserve( c_include_path.size() + args.size() + 2 );
    all_args.push_back( "-specs=/__gg__/gcc-specs" );
    all_args.push_back( "-nostdinc" );
    for ( const auto & p : c_include_path ) {
      all_args.push_back( "-isystem" + p );
    }

    all_args.insert( all_args.end(), args.begin(), args.end() );

    return { output, gcc_function( all_args, envars_ ), base_infiles };
  }

  case COMPILE:
    args.push_back( "-S" );
    args = prune_makedep_flags( args );
    base_infiles.push_back( program_infiles.at( CC1 ) );

    return { output, gcc_function( args, envars_ ), base_infiles };

  case ASSEMBLE:
    args.push_back( "-c" );
    args = prune_makedep_flags( args );
    base_infiles.push_back( program_infiles.at( AS ) );
    return { output, gcc_function( args, envars_ ), base_infiles };

  default: throw runtime_error( "not implemented" );
  }
}

Thunk GCCModelGenerator::generate_link_thunk( const vector<InputFile> & link_inputs,
                                              const vector<string> & link_args,
                                              const vector<string> & dependencies,
                                              const string & output )
{
  vector<InFile> infiles;
  infiles.emplace_back( program_infiles.at( GCC ) );
  infiles.emplace_back( program_infiles.at( COLLECT2 ) );
  infiles.emplace_back( program_infiles.at( LD ) );

  for ( auto const & link_input : link_inputs ) {
    if ( link_input.source_language == Language::OBJECT or
         link_input.source_language == Language::ARCHIVE_LIBRARY ) {
      infiles.emplace_back( link_input.name );
    }
  }

  for ( const string & dep : dependencies ) {
    infiles.emplace_back( dep );
  }

  vector<string> args;
  args.reserve( link_args.size() + gcc_library_path.size() );

  for ( const string & dir : gcc_library_path ) {
    infiles.emplace_back( dir, InFile::Type::DUMMY_DIRECTORY );
    args.push_back( "-L" + dir );
  }

  for ( const string & dir : ld_search_path ) {
    infiles.emplace_back( dir, InFile::Type::DUMMY_DIRECTORY );
  }

  infiles.emplace_back( gcc_install_path, InFile::Type::DUMMY_DIRECTORY );

  args.insert( args.end(), link_args.begin(), link_args.end() );

  args.push_back( "-B" + gcc_install_path );

  return { output, gcc_function( args, envars_ ), infiles };
}

GCCModelGenerator::GCCModelGenerator( const OperationMode operation_mode,
                                      int argc, char ** argv )
  : operation_mode_( operation_mode ), arguments_( argc, argv )
{
  if ( arguments_.input_files().size() == 0 ) {
    throw runtime_error( "no input files" );
  }

  dump_gcc_specs( specs_tempfile_ );

  size_t non_object_inputs = std::count_if( arguments_.input_files().begin(),
                                            arguments_.input_files().end(),
                                            is_non_object_input );

  if ( non_object_inputs > 0 && arguments_.input_files().size() > 1 ) {
    throw runtime_error( "multiple inputs are only allowed for linking" );
  }
}

void GCCModelGenerator::generate()
{
  roost::path gg_dir = gg::models::create_gg_dir();

  string last_stage_output_filename = arguments_.output_filename();
  GCCStage last_stage = arguments_.last_stage();
  vector<string> args = arguments_.all_args();
  const vector<InputFile> & input_files = arguments_.input_files();

  vector<InputFile> link_inputs;

  for ( const InputFile & input : input_files ) {
    if ( !is_non_object_input( input ) ) {
      link_inputs.push_back( input );
      continue;
    }

    GCCStage first_stage = language_to_stage( input.language );
    GCCStage input_last_stage = ( last_stage == LINK ) ? ASSEMBLE : last_stage;
    string input_hash = digest::SHA256( input.name ).hexdigest();

    map<size_t, string> stage_output;
    stage_output[ first_stage - 1 ] = input.name;

    for ( size_t stage_num = first_stage; stage_num <= input_last_stage; stage_num++ ) {
      GCCStage stage = static_cast<GCCStage>( stage_num );

      string output_name = ( stage == last_stage ) ? last_stage_output_filename
                                                   : stage_output_name( stage, input_hash );

      vector<string> args_stage { args };
      args_stage[ input.index ] = stage_output[ stage - 1 ];
      Thunk stage_thunk = generate_thunk( stage, args_stage, stage_output[ stage - 1 ],
                                          output_name );

      stage_thunk.store( gg_dir );

      switch ( stage ) {
      case PREPROCESS:
        /* generate preprocess thunk */
        cerr << ">> preprocessing " << stage_output[ stage - 1 ] << endl;
        break;

      case COMPILE:
        /* generate compile thunk */
        cerr << ">> compiling " << stage_output[ stage - 1 ] << endl;
        break;

      case ASSEMBLE:
      {
        /* generate assemble thunk */
        cerr << ">> assembling " << stage_output[ stage - 1 ] << endl;

        InputFile link_input = input;
        link_input.name = output_name;
        link_input.language = Language::OBJECT;
        link_input.source_language = input.language;
        link_input.index = input.index;
        link_inputs.push_back( link_input );

        break;
      }

      default:
        throw runtime_error( "unexcepted stage" );
      }

      stage_output[ stage ] = output_name;
      cerr << "[output=" << output_name << "]" << endl;
    }
  }

  if ( last_stage == LINK ) {
    if ( last_stage_output_filename.length() == 0 ) {
      last_stage_output_filename = "a.out";
    }

    vector<string> link_args { args };

    cerr << ">> linking ";
    for ( auto const & link_input : link_inputs ) {
      if ( not ( link_input.source_language == Language::OBJECT or
                 link_input.source_language == Language::ARCHIVE_LIBRARY or
                 link_input.source_language == Language::SHARED_LIBRARY ) ) {
        link_args[ link_input.index ] = link_input.name;
      }

      cerr << link_input.name << " ";
    }
    cerr << endl;

    vector<string> dependencies = get_link_dependencies( link_inputs, args );

    link_args.push_back( "-Wl,-rpath-link,/lib/x86_64-linux-gnu" );

    Thunk thunk = generate_link_thunk( link_inputs, link_args,
                                       dependencies,
                                       last_stage_output_filename );
    thunk.store( gg_dir );
  }
}

void usage( const char * arg0 )
{
  cerr << arg0 << " (gcc|g++) [GCC ARGUMETNS]" << endl;
}

int main( int argc, char * argv[] )
{
  try {
    if ( argc <= 0 ) {
      abort();
    }

    if ( argc < 3 ) {
      usage( argv[ 0 ] );
      return EXIT_FAILURE;
    }

    OperationMode operation_mode;

    if ( strcmp( argv[ 1 ], "gcc" ) == 0 ) {
      operation_mode = OperationMode::GCC;
    }
    else if ( strcmp( argv[ 1 ], "g++" ) == 0 ) {
      operation_mode = OperationMode::GXX;
      throw runtime_error( "not implemented" );
    }
    else {
      throw runtime_error( "invalid operation mode" );
    }

    argv++;
    argc--;

    GCCModelGenerator gcc_model_generator { operation_mode, argc, argv };
    gcc_model_generator.generate();

    return EXIT_SUCCESS;
  }
  catch ( const exception & e ) {
    print_exception( argv[ 0 ], e );
    return EXIT_FAILURE;
  }
}
