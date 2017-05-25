#include "gg-model-base.hh"

#include <iostream>

using namespace std;
using namespace gg::thunk;

class GGModelCompile : public GGModelBase{
protected:
  Function get_function() {
    return Function(vector<string>());
  };

  string get_outfile() {
    return NULL;
  };

  vector<InFile> get_infiles() {
    return vector<InFile>();
  };
};

int main(){
  cout << "howdy" << endl;
}