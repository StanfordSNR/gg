#include <iostream>
#include <fstream>
#include <string.h>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cout << "Usage: test_program <input file> <line number to read>" << endl;
        return 1;
    }

    string ifile = argv[1];
    string out_num_str = argv[2];
    int out_num = atoi(argv[2]);

    ifstream my_infile;
    my_infile.open( ifile );
    string read_line;
    for (int i = 0; i <= out_num; ++i) {
        my_infile >> read_line;
    }


    ofstream my_ofile;
    my_ofile.open("test_" + out_num_str + ".out");
    my_ofile << "Thunk " << out_num << " read: " << read_line << "\n";
    my_ofile.close();

    return 0;
}
    
