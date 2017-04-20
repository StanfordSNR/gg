#include "thunk_writer.hh"
#include <iostream>

bool ThunkWriter::write_thunk(Thunk thunk) {
    cout << thunk.to_json() << endl;
    return true;
}
