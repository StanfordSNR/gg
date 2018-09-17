/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <iostream>
#include <algorithm>

#include "gcc.hh"
#include "timeouts.hh"
#include "thunk/thunk.hh"
#include "thunk/thunk_reader.hh"
#include "thunk/thunk_writer.hh"
#include "thunk/ggutils.hh"
#include "util/exception.hh"
#include "util/system_runner.hh"
#include "util/tokenize.hh"
#include "util/util.hh"

using namespace std;
using namespace gg::thunk;

void usage( const char * argv0 )
{
  cerr << argv0 << " PREPROCESSOR-THUNK" << endl;
}

vector<string> canonicalize_args( vector<string> & args )
{
  const int argc = args.size();
  vector<char *> argv;
  transform( args.begin(), args.end(), back_inserter( argv ),
             []( string & s ) { return &s[ 0 ]; } );

  GCCArguments gcc_arguments { argc, &argv[ 0 ], false, true };
  vector<string> new_args = gcc_arguments.all();

  return new_args;
}

void prepare_include_path( const roost::path & sysroot,
                           const vector<string> & tarballs )
{
  roost::create_directories( sysroot );

  /* (1) extract the tarballs */
  for ( const string & tarball_hash : tarballs ) {
    vector<string> extract_args { "/bin/tar", "xf",
                                  gg::paths::blob( tarball_hash ).string(),
                                  "-C", sysroot.string(), "-I", "/bin/gzip" };

    cerr << "Extracting " << tarball_hash << "...";
    run( extract_args[ 0 ], extract_args, {}, true, true );
    cerr << "done." << endl;
  }
}

string get_hash( const string & file )
{
  constexpr char PREFIX[] = "// GGHASH:";
  constexpr size_t PREFIX_LEN = sizeof( PREFIX ) - 1;
  constexpr size_t LEN = PREFIX_LEN + gg::hash::length;
  char read_buffer[ LEN ];

  ifstream fin { file };
  fin.read( read_buffer, LEN );

  if ( strncmp( PREFIX, read_buffer, PREFIX_LEN ) == 0 ) {
    return { read_buffer + PREFIX_LEN, gg::hash::length };
  }
  else {
    return gg::hash::file_force( file );
  }
}

int main( int argc, char * argv[] )
{
  try {
    if ( argc <= 0 ) {
      abort();
    }

    if ( argc < 2 ) {
      usage( argv[ 0 ] );
      return EXIT_FAILURE;
    }

    /* setting the GG_DIR, so we can find the blobs directory */
    setenv( "GG_DIR",
            roost::dirname( safe_getenv( "__GG_DIR__" ) ).string().c_str(),
            true );

    Thunk thunk = ThunkReader::read( safe_getenv( "__GG_THUNK_PATH__" ) );

    const roost::path execution_root { roost::current_working_directory() };

    /* read all the necessary environment variables *****************/
    roost::path working_directory  = safe_getenv( DEPGEN_WORKING_DIRECTORY );
    const string include_tarballs  = safe_getenv( DEPGEN_INCLUDE_TARBALLS );
    const string input_name        = safe_getenv( DEPGEN_INPUT_NAME );
    const string target_name       = safe_getenv( DEPGEN_TARGET_NAME );
    const string gcc_function_hash = safe_getenv( DEPGEN_GCC_HASH );
    const string cc1_function_name = safe_getenv( DEPGEN_CC1_NAME );
    const string cc1_function_hash = safe_getenv( DEPGEN_CC1_HASH );

    /* create the sysroot directory *********************************/
    roost::create_directories( GG_SYSROOT_PREFIX );
    const roost::path sysroot = roost::canonical( GG_SYSROOT_PREFIX );

    /* parse the arguments ******************************************/
    vector<string> thunk_args = thunk.function().args();

    const OperationMode op_mode =
      ( thunk_args[ 0 ] == program_data.at( GCC ).filename() )
      ? OperationMode::GCC : OperationMode::GXX;

    thunk_args.erase( thunk_args.begin() + 1 );
    thunk_args = canonicalize_args( thunk_args );

    /* prepare the working directory ********************************/
    working_directory = sysroot / working_directory;
    roost::create_directories( working_directory );

    /* put gcc binaries in place ************************************/
    const roost::path gcc_dir = sysroot / "gccbin";
    roost::create_directories( gcc_dir );
    const string path_envar = "PATH=" + gcc_dir.string();
    const roost::path gcc_path = gcc_dir / ( ( op_mode == OperationMode::GCC ) ? "gcc" : "g++" );
    const roost::path cc1_path = gcc_dir / cc1_function_name;
    roost::symlink( gg::paths::blob( gcc_function_hash ), gcc_path );
    roost::symlink( gg::paths::blob( cc1_function_hash ), cc1_path );

    /* extract the tarballs containig stripped header files *********/
    const vector<string> tarballs = split( include_tarballs, ":" );
    prepare_include_path( sysroot, tarballs );

    roost::chdir( working_directory );

    /* put all the extra files in the sysroot ***********************/
    for ( const auto & value : thunk.values() ) {
      if ( value.second.length() == 0 or
           value.second.compare( 0, sizeof( "/__gg" ) - 1, "/__gg" ) == 0 ) {
        continue;
      }

      roost::path filepath { value.second };
      if ( roost::is_absolute( filepath ) ) {
        filepath = sysroot / filepath;
      }

      cerr << "Putting file " << value.second << "...";
      roost::create_directories( roost::dirname( filepath ) );
      if ( roost::exists( filepath ) ) { roost::remove( filepath ); }
      roost::symlink( gg::paths::blob( value.first ), filepath );
      cerr << "done." << endl;
    }

    /* time to run gcc -M *******************************************/
    const vector<string> dependencies =
      GCCModelGenerator::generate_dependencies_file(
        op_mode, input_name, thunk_args,
        ( execution_root / "dependencies" ).string(), target_name,
        false, gcc_path.string(), vector<string> { { path_envar } } );

    /* (1) create a new function ###################################*/
    const Function & function = thunk.function();
    vector<string> new_envars;

    for ( const string & envar : function.envars() ) {
      if ( envar.compare( 0, sizeof( "_GG" ) - 1, "_GG" ) != 0 ) {
        new_envars.push_back( envar );
      }
    }

    Function new_function { gcc_function_hash, function.args(), new_envars };

    /* (2) create new data lists ###################################*/
    Thunk::DataList values = thunk.values();
    Thunk::DataList executables = thunk.executables();

    executables.erase( function.hash() );

    for ( const auto & tarball_hash : tarballs ) {
      values.erase( tarball_hash );
    }

    for ( auto it = values.begin(); it != values.end(); ) {
      if ( it->second.length() == 0 or
           it->second == "/__gg__/gcc-specs" ) {
        it++;
        continue;
      }

      it = values.erase( it );
    }

    for ( const auto & dep : dependencies ) {
      const string dep_hash = get_hash( dep );
      string dep_path;

      if ( dep.compare( 0, sysroot.string().length(), sysroot.string() ) == 0 ) {
        dep_path = dep.substr( sysroot.string().length() );
      }
      else {
        dep_path = dep;
      }

      dep_path = roost::path( dep_path ).lexically_normal().string();
      values.emplace( make_pair( dep_hash, dep_path ) );
    }

    /* (3) create a new thunk ######################################*/
    Thunk new_thunk { move( new_function ), move( values ), {},
                      move( executables ), { "output" } };

    new_thunk.set_timeout( PREPROCESS_TIMEOUT );

    /* (4) clean up and write output ###############################*/
    roost::remove_directory( sysroot );
    ThunkWriter::write( new_thunk, execution_root / "output" );
  }
  catch ( const exception &  e ) {
    print_exception( argv[ 0 ], e );
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
