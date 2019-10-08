#include <fstream>
#include <string>
#include <thread>
#include <chrono>

using namespace std;

int main(int argc, char * argv[]) {
  if (argc != 3) {
    return EXIT_FAILURE;
  }

  int sleep_time = stoi(argv[1]);
  this_thread::sleep_for(chrono::seconds{sleep_time});

  ofstream fout{"output"};
  fout << argv[2] << endl;

  return EXIT_SUCCESS;
}
