#include <fstream>
#include <cstring>

using namespace std;

bool is_thunk(const string & path) {
  constexpr char magic_number[] = "##GGTHUNK##";
  constexpr size_t magic_number_len = sizeof(magic_number) / sizeof(char) - 1;
  char first_bytes[magic_number_len] = {0};

  ifstream fin{path};
  fin.read(first_bytes, magic_number_len);

  return (strncmp(magic_number, first_bytes, magic_number_len) == 0);
}

int main(int argc, char * argv[]) {
  for (int i = 1; i < argc; i++) {
    if (not is_thunk(argv[i])) {
      ifstream fin{argv[i]};
      ofstream fout{"output"};

      fout << fin.rdbuf();
      return EXIT_SUCCESS;
    }
  }

  ifstream tin{getenv("__GG_THUNK_PATH__")};
  ofstream tout{"output"};
  tout << tin.rdbuf();

  return EXIT_SUCCESS;
}
