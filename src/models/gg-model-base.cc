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

string safe_getenv(const char *flag){
  char * var = getenv( flag );
  if( var == NULL ) {
    throw runtime_error("You must specify a GG directory" );
  }
  return string(var);
}

GGModelBase::GGModelBase() :
  GG_DIR( safe_getenv(GG_DIR_FLAG.c_str()) )
{}

GGModelBase::~GGModelBase(){}

Thunk GGModelBase::build_thunk() {

  Function thunk_func = get_function();
  vector<InFile> infiles = get_infiles();
  copy_infiles_to_gg(infiles);
  string outfile = get_outfile();
  Thunk thunk { outfile, thunk_func, infiles };

  return thunk;
}

void GGModelBase::write_thunk() {

  Thunk thunk = build_thunk();
  ThunkWriter::write_thunk( thunk );

}

void GGModelBase::copy_infiles_to_gg(std::vector<gg::thunk::InFile> & infiles){
  for( InFile infile : infiles ){
    ifstream  src( infile.filename(), ios::binary );
    ofstream  dst( GG_DIR + infile.hash(),  ios::binary );
    struct stat fst;
    stat( infile.filename().c_str(), &fst );
    chmod( ( GG_DIR + infile.hash()).c_str(), fst.st_mode );
    dst << src.rdbuf();
  }
}
