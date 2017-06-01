/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "gg-model-base.hh"

#include <iostream>
#include <unistd.h>
#include <getopt.h>

using namespace std;
using namespace gg::thunk;

static const string GCC_COMPILER = ".gg/exe/bin/x86_64-linux-musl-gcc";
static const string AS = ".gg/exe/bin/as";

class GGModelAssemble : public GGModelBase
{
private:
  string srcfile {};
  string outfile {};
  vector<string> cmd {};

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

protected:
  Function get_function() { return Function(cmd); }

  string get_outfile()
  {
    if ( outfile.empty() ) {
      throw runtime_error( "Command line parameters were not parsed in model constructor" );
    }

    return outfile;
  };

  vector<InFile> get_infiles()
  {
    vector<InFile> infiles { srcfile, GCC_COMPILER, AS };
    return infiles;
  }

public:
  GGModelAssemble( int argc, char **argv )
    : GGModelBase()
  {
    store_args(argc, argv);
    parse_args(argc, argv);
  }

  ~GGModelAssemble() {}

  string get_srcfile( int argc, char ** argv ) {
    if ( srcfile.empty() ) {
      parse_args( argc, argv );
    }
    return srcfile;
  }
};

int main( int argc, char ** argv )
{
GGModelAssemble model { argc, argv };
  model.write_thunk();
}
