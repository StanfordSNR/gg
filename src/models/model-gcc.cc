/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "model-gcc.hh"

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
#include <getopt.h>
#include <libgen.h>
#include <sys/ioctl.h>

#include "thunk/factory.hh"
#include "thunk/ggutils.hh"
#include "thunk/thunk.hh"
#include "util/digest.hh"
#include "util/exception.hh"
#include "util/optional.hh"
#include "util/system_runner.hh"
#include "util/temp_file.hh"

using namespace std;
using namespace gg::thunk;

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

string GCCModelGenerator::generate_thunk( const GCCStage first_stage,
                                          const GCCStage stage,
                                          const InputFile & input,
                                          const string & output,
                                          const bool write_placeholder )
{
  vector<string> args { arguments_.option_args() };

  args.erase(
    remove_if(
      args.begin(), args.end(),
      []( const string & s ) { return ( s == "-E" or s == "-S" or s == "-c" ); }
    ), args.end()
  );

  /* Common infiles */
  vector<ThunkFactory::Data> base_infiles = {
    input.indata,
    program_data.at( ( operation_mode_ == OperationMode::GCC ) ? GCC : GXX )
  };

  base_infiles.emplace_back( "/__gg__/gcc-specs", specs_tempfile_.name() );

  for ( const string & extra_infile : arguments_.extra_infiles( stage ) ) {
    base_infiles.emplace_back( extra_infile );
  }

  /* Common dummy directories */
  vector<string> dummy_dirs;

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
      ( input.language == Language::C or
        input.language == Language::C_HEADER or
        input.language == Language::ASSEMBLER_WITH_CPP )
        ? c_include_path
        : cpp_include_path;

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

    if ( not arguments_.no_stdinc() ) {
      all_args.push_back( "-nostdinc" );

      for ( const auto & p : include_path ) {
        all_args.push_back( "-isystem" + p );
      }

      if ( input.language == Language::CXX or
           input.language == Language::CXX_HEADER ) {
        all_args.push_back( "-nostdinc++" );
      }
    }

    all_args.insert( all_args.end(), args.begin(), args.end() );

    /* Generate dependency list */
    TempFile makedep_tempfile { "/tmp/gg-makedep" };
    string makedep_filename;
    string makedep_target = DEFAULT_MAKE_TARGET;

    if ( generate_makedep_file ) {
      cerr << "\u251c\u2500 generating make dependencies file... ";
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
      cerr << "done." << endl;
    }
    else {
      makedep_filename = makedep_tempfile.name();
    }

    const vector<string> dependencies = generate_dependencies_file( all_args,
                                                                    makedep_filename, makedep_target );

    /* We promised that we would add these here, and we lived up to our
       promise... */
    all_args.insert( all_args.begin(), "-specs=/__gg__/gcc-specs" );
    all_args.push_back( "-o" );
    all_args.push_back( output );
    all_args = prune_makedep_flags( all_args );

    /* INFILES */
    if ( input.language == Language::C or
         input.language == Language::C_HEADER or
         input.language == Language::ASSEMBLER_WITH_CPP ) {
      base_infiles.emplace_back( program_data.at( CC1 ) );
    }
    else {
      base_infiles.emplace_back( program_data.at( CC1PLUS ) );
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

    dummy_dirs.push_back( "." );

    return ThunkFactory::generate(
      gcc_function( operation_mode_, all_args, envars_ ),
      base_infiles,
      { { "output", output } },
      true, dummy_dirs, write_placeholder
    );
  }

  case COMPILE:
    args.push_back( "-S" );

    args.push_back( "-frandom-seed=__GG_BEGIN_REPLACE__"
                    + input.name
                    + "__GG_END_REPLACE__" );

    args = prune_makedep_flags( args );

    if ( input.language == Language::CPP_OUTPUT ) {
      base_infiles.push_back( program_data.at( CC1 ) );
    }
    else {
      base_infiles.push_back( program_data.at( CC1PLUS ) );
    }

    return ThunkFactory::generate(
      gcc_function( operation_mode_, args, envars_ ),
      base_infiles,
      { { "output", output } },
      true, dummy_dirs, write_placeholder
    );

  case ASSEMBLE:
    if ( first_stage != ASSEMBLE and
         ( arguments_.option_argument( GCCOption::gdwarf_4 ).initialized() or
           arguments_.option_argument( GCCOption::g ).initialized() ) ) {
      args.erase(
        remove_if(
          args.begin(), args.end(),
          []( const string & s ) { return ( s == "-gdwarf-4" or s == "-g" ); }
        ), args.end()
      );
    }

    args.push_back( "-c" );
    args = prune_makedep_flags( args );
    base_infiles.push_back( program_data.at( AS ) );

    return ThunkFactory::generate(
      gcc_function( operation_mode_, args, envars_ ),
      base_infiles,
      { { "output", output } },
      true, dummy_dirs, write_placeholder
    );

  default: throw runtime_error( "not implemented" );
  }
}

void print_gcc_command( const string & command_str )
{
  struct winsize size;
  ioctl( STDOUT_FILENO, TIOCGWINSZ, &size );
  size_t window_width = size.ws_col - 5;

  cerr << "\u256d\u257c generating model for:" << endl;
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
                                      int argc, char ** argv )
  : operation_mode_( operation_mode ), arguments_( argc, argv, force_strip )
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
  vector<string> args = arguments_.all_args();

  vector<InputFile> input_files = arguments_.input_files();

  /* count non-object input files */
  const size_t source_inputs = std::count_if( input_files.begin(),
                                              input_files.end(),
                                              []( const InputFile & input ) {
                                                return is_non_object_input( input );
                                              } );

  if ( source_inputs > 1 and not final_output.empty() ) {
    throw runtime_error( "cannot specify -o with -c, -S or -E with multiple files" );
  }

  for ( size_t input_index = 0; input_index < input_files.size(); input_index++ ) {
    InputFile & input = input_files.at( input_index );

    if ( not is_non_object_input( input ) ) {
      continue;
    }

    GCCStage first_stage = language_to_stage( input.language );
    GCCStage input_last_stage = ( last_stage == LINK ) ? ASSEMBLE : last_stage;

    map<size_t, string> stage_output;
    stage_output[ first_stage - 1 ] = input.name;
    input.indata = move( ThunkFactory::Data( input.name ) );

    cerr << "\u251c\u257c input: " << input.name << endl;

    for ( size_t stage_num = first_stage; stage_num <= input_last_stage; stage_num++ ) {
      GCCStage stage = static_cast<GCCStage>( stage_num );
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
            final_output = stage_output_name( stage,
                                              split_source_name( arguments_.input_files().at( input_index ).name ).first );
          }
        }

        output_name = final_output;
      }
      else {
        output_name = "output_" + to_string( input_index )
                                + "_" + to_string( stage_num );
      }

      string last_stage_hash = generate_thunk( first_stage, stage, input, output_name, stage == last_stage );

      switch ( stage ) {
      case PREPROCESS:
        /* generate preprocess thunk */
        cerr << "\u251c\u2500 preprocessed: " << last_stage_hash << endl;

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
      input.indata = ThunkFactory::Data( "nonexistent", input.name,
                                         gg::ObjectType::Value, last_stage_hash );

      if ( stage == last_stage ) {
        cerr << "\u2570\u257c output: " << final_output << endl;
      }
    }
  }

  if ( last_stage == LINK ) {
    if ( final_output.length() == 0 ) {
      final_output = "a.out";
    }

    vector<string> link_args { args };

    vector<string> dependencies = get_link_dependencies( input_files, args );

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

    print_gcc_command( command_str( argc, argv ) );

    GCCModelGenerator gcc_model_generator { operation_mode, argc, argv };
    gcc_model_generator.generate();

    return EXIT_SUCCESS;
  }
  catch ( const exception & e ) {
    print_exception( argv[ 0 ], e );
    return EXIT_FAILURE;
  }
}
