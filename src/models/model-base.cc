/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "model-base.hh"
#include "thunk_writer.hh"

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>

using namespace std;
using namespace gg::thunk;

const string ModelBase::GG_DIR_FLAG = "GG_DIR";
static const string GCC_COMPILER = ".gg/exe/bin/x86_64-linux-musl-gcc";

string safe_getenv( const char * flag )
{
  char * var = getenv( flag );

  if ( var == NULL ) {
    throw runtime_error( "You must specify a GG directory" );
  }

  return string( var );
}

string srcfile {};
string outfile {};
vector<string> cmd {};

Thunk ModelBase::build_thunk()
{
  Function thunk_func = get_function();
  vector<InFile> infiles = get_infiles();
  copy_infiles_to_gg( infiles );
  string outfile = get_outfile();
  Thunk thunk { outfile, thunk_func, infiles };

  return thunk;
}

void ModelBase::write_thunk()
{
  Thunk thunk = build_thunk();
  ThunkWriter::write_thunk( thunk );
}

void ModelBase::copy_infiles_to_gg( vector<InFile> & infiles )
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

string ModelBase::get_srcfile( int argc, char ** argv ) {
  if ( srcfile.empty() ) {
    parse_args( argc, argv );
  }
  return srcfile;
}

void ModelBase::parse_args( int argc, char ** argv )
{
  optind = 1; /* reset getopt */
  opterr = 0; /* turn off error messages */

  char arg;
  while ( ( arg = getopt( argc, argv, "-o:" ) ) != -1 ) {
    switch ( arg ) {
    case 1:
      srcfile = string( optarg );
      break;

    case 'o':
      outfile = string( optarg );
      break;
    }
  }
}

void ModelBase::store_args( int argc, char **argv )
{
  for ( int i = 0; i < argc; i++ ) {
    if ( i == 0 ) {
      cmd.push_back( GCC_COMPILER );
    }
    else {
      cmd.push_back( string( argv[ i ] ) );
    }
  }
}

Function ModelBase::get_function() { return Function(cmd); }

string ModelBase::get_outfile()
{
  if ( outfile.empty() ) {
    throw runtime_error( "Command line parameters were not parsed in model constructor" );
  }

  return outfile;
}

ModelBase::ModelBase( int argc, char ** argv )
  : GG_DIR( safe_getenv( GG_DIR_FLAG.c_str() ) )
{
  store_args( argc, argv );
  parse_args( argc, argv );
}

ModelBase::ModelBase( const std::vector<std::string> & args )
  : GG_DIR( safe_getenv( GG_DIR_FLAG.c_str() ) )
{
  char ** argv = new char * [ args.size() + 1 /* NULL at the end */ ];

  for ( size_t i = 0; i < args.size(); i++ ) {
    argv[ i ] = new char[ args[ i ].length() + 1 ];
    args[ i ].copy( argv[ i ], args[ i ].length() );
    argv[ i ][ args[ i ].length() ] = '\0';
  }

  argv[ args.size() ] = NULL;

  store_args( args.size(), argv );
  parse_args( args.size(), argv );

  for ( size_t i = 0; i < args.size(); i++ ) {
    delete[] argv[ i ];
  }

  delete[] argv;
}

ModelBase::~ModelBase()
{}
