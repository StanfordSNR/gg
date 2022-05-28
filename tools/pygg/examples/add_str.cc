#include <iostream>
#include <fstream>

using namespace std;

int main (int argc, char *argv[]) {
  int sum = 0;
  for (int i = 1; i < argc; ++i) {
    ifstream f{argv[i]};
    int x;
    f >> x;
    if (f.bad()) {
      throw runtime_error("Bad input!");
    }
    sum += x;
  }
  ofstream f{"out"};
  f << sum;
  f.close();
}
