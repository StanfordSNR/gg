/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "gg-model-base.hh"
#include "thunk_writer.hh"

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>

using namespace std;
using namespace gg::thunk;

const string GGModelBase::GG_DIR_FLAG = "GG_DIR";

string safe_getenv( const char *flag )
{
  char * var = getenv( flag );

  if ( var == NULL ) {
    throw runtime_error( "You must specify a GG directory" );
  }

  return string( var );
}

class GGModelBase 
{
  protected:
    string srcfile {};
    string outfile {};
    vector<string> cmd {};

    Thunk build_thunk()
    {
      Function thunk_func = get_function();
      vector<InFile> infiles = get_infiles();
      copy_infiles_to_gg( infiles );
      string outfile = get_outfile();
      Thunk thunk { outfile, thunk_func, infiles };

      return thunk;
    }

    void write_thunk()
    {
      Thunk thunk = build_thunk();
      ThunkWriter::write_thunk( thunk );
    }

    void copy_infiles_to_gg( vector<InFile> & infiles )
    {
      for ( InFile infile : infiles ) {
        ifstream src( infile.filename(), ios::binary );
        ofstream dst( GG_DIR + infile.hash(), ios::binary );
        struct stat fst;
        stat( infile.filename().c_str(), &fst );
        chmod( ( GG_DIR + infile.hash()).c_str(), fst.st_mode );
        dst << src.rdbuf();
      }
    }

    string get_srcfile( int argc, char ** argv ) {
      if ( srcfile.empty() ) {
        parse_args( argc, argv );
      }
      return srcfile;
    }

    void parse_args( int argc, char **argv )
    {
      char arg;
      while ( ( arg = getopt( argc, argv, "gScO:f:o:" ) ) != -1 ) {
        switch ( arg ) {
        case 'o':
          outfile = string( optarg );
          break;
        }
      }
      srcfile = argv[ optind ];
    }

    void store_args( int argc, char **argv )
    {
      for ( int i = 0; i < argc; i++ ){
        if ( i == 0 ) {
          cmd.push_back( GCC_COMPILER );
        }
        else {
          cmd.push_back( string( argv[i] ) );
        }
      }
    }

    Function get_function() { return Function(cmd); }

    string get_outfile()
    {
      if ( outfile.empty() ) {
        throw runtime_error( "Command line parameters were not parsed in model constructor" );
      }

      return outfile;
    }

  public:
    GGModelBase::GGModelBase( int argc, char **argv )
      : GG_DIR( safe_getenv( GG_DIR_FLAG.c_str() ) )
    {
      store_args(argc, argv);
      parse_args(argc, argv);
    }

    GGModelBase::~GGModelBase()
    {}
};
