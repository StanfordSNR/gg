#include "gg-model-base.hh"

#include <iostream>
#include <unistd.h>
#include <getopt.h>

using namespace std;
using namespace gg::thunk;


class GGModelCompile : public GGModelBase{
private:
  string srcfile{};
  string outfile{};

  void parse_args(int argc, char **argv){
    char arg;
    while ((arg = getopt( argc, argv, "gScO:f:o:" )) != -1){
      switch(arg){
        case 'o':
          outfile = string(optarg);
          break;
      }
    }
    srcfile = argv[optind];
  };

protected:
  Function get_function() {
    return Function(vector<string>());
  };

  string get_outfile() {
    if (outfile.empty()){
      throw runtime_error( "Command line parameters were"
                               " not parsed in model constructor" );
    }
    return outfile;
  };

  vector<InFile> get_infiles() {
    return vector<InFile>();
  };



public:

  GGModelCompile( int argc, char **argv ) : GGModelBase(){
    parse_args(argc, argv);
  }

  ~GGModelCompile(){}

  string get_srcfile(int argc, char **argv){
    if (srcfile.empty()){
      parse_args(argc, argv);
    }
    return srcfile;
  }
};

int main( int argc, char **argv ){
  GGModelCompile model { argc, argv };
  cout << model.get_srcfile(argc, argv) << endl;
  // model.write_thunk();
}