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

Thunk GCCModelGenerator::generate_thunk( const GCCStage stage,
                                         const InputFile & input,
                                         const string & output )
{
  vector<string> args { arguments_.option_args() };

  args.erase(
    remove_if(
      args.begin(), args.end(),
      []( const string & s ) { return ( s == "-E" or s == "-S" or s == "-c" ); }
    ), args.end()
  );

  /* Common infiles */
  vector<InFile> base_infiles = {
    input.name,
    program_infiles.at( ( operation_mode_ == OperationMode::GCC ) ? GCC : GXX )
  };

  base_infiles.emplace_back( "/__gg__/gcc-specs", specs_tempfile_.name() );

  /* Common args */
  args.push_back( "-x" );
  args.push_back( language_to_name( input.language ) );
  args.push_back( input.name );

  if ( stage != PREPROCESS ) {
    /* For preprocess stage, we are going to use `args` to get the
       dependencies, so we can't have this FAKE specs path here.
       We will add these later.*/
    args.insert( args.begin(), "-specs=/__gg__/gcc-specs" );
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
    const vector<string> & include_path =
      ( input.language == Language::C or input.language == Language::C_HEADER )
        ? c_include_path
        : cpp_include_path;

     /* ARGS */
    args.push_back( "-E" );

    vector<string> all_args;
    all_args.reserve( c_include_path.size() + args.size() + 2 );
    all_args.push_back( "-nostdinc" );

    if ( operation_mode_ == OperationMode::GXX ) {
      all_args.push_back( "-nostdinc++" );
    }

    for ( const auto & p : include_path ) {
      all_args.push_back( "-isystem" + p );
    }

    all_args.insert( all_args.end(), args.begin(), args.end() );

    /* Generate dependency list */
    TempFile makedep_tempfile { "/tmp/gg-makedep" };
    string makedep_filename;
    string makedep_target = input.name;

    if ( generate_makedep_file ) {
      cerr << "[+] generating make dependencies file..." << endl;
      generate_dependencies_file( all_args, input.name, "" );
      makedep_filename = *arguments_.option_argument( GCCOption::MF );
      Optional<string> mt_arg = arguments_.option_argument( GCCOption::MT );
      if ( mt_arg.initialized() ) {
        makedep_target = *mt_arg;
      }
    }
    else {
      generate_dependencies_file( args, input.name, makedep_tempfile.name() );
      makedep_filename = makedep_tempfile.name();
    }

    vector<string> dependencies = parse_dependencies_file( makedep_filename, makedep_target );

    all_args.insert( all_args.begin(), "-specs=/__gg__/gcc-specs" );
    all_args.push_back( "-o" );
    all_args.push_back( output );
    all_args = prune_makedep_flags( all_args );

    /* INFILES */
    base_infiles.emplace_back( program_infiles.at( CC1 ) );
    for ( const string & dep : dependencies ) {
      base_infiles.emplace_back( dep );
    }

    for ( const string & dir : include_path ) {
      base_infiles.emplace_back( dir, InFile::Type::DUMMY_DIRECTORY );
    }

    for ( const string & dir : arguments_.include_dirs() ) {
      base_infiles.emplace_back( dir, InFile::Type::DUMMY_DIRECTORY );
    }

    base_infiles.emplace_back( ".", InFile::Type::DUMMY_DIRECTORY );

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

  vector<InputFile> input_files = arguments_.input_files();

  vector<InputFile> link_inputs;

  for ( InputFile & input : input_files ) {
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

      Thunk stage_thunk = generate_thunk( stage, input, output_name );

      stage_thunk.store( gg_dir );

      switch ( stage ) {
      case PREPROCESS:
        /* generate preprocess thunk */
        cerr << ">> preprocessing " << stage_output[ stage - 1 ] << endl;

        switch ( input.language ) {
        case Language::C:
        case Language::C_HEADER:
          input.language = Language::CPP_OUTPUT;
          break;

        case Language::CXX_HEADER:
        case Language::CXX:
          input.language = Language::CXX_CPP_OUTPUT;
          break;

        default:
          throw runtime_error( "invalid preprocessing language" );
        }

        break;

      case COMPILE:
        /* generate compile thunk */
        cerr << ">> compiling " << stage_output[ stage - 1 ] << endl;
        input.language = Language::ASSEMBLER;
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

      input.name = output_name;
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
