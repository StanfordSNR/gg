#include "gg-model-base.hh"
#include "thunk_writer.hh"

#include <cstdlib>

using namespace std;
using namespace gg::thunk;

GGModelBase::GGModelBase() :
  GG_DIR( getenv( GG_DIR_FLAG ) )
{}

Thunk GGModelBase::build_thunk() {

  Function thunk_func = get_function();
  vector<InFile> infiles = get_infiles();
  string outfile = get_outfile();
  Thunk thunk { outfile, thunk_func, infiles };

  return thunk;
}

void GGModelBase::write_thunk() {

  Thunk thunk = build_thunk();
  ThunkWriter::write_thunk( thunk );
  
}

