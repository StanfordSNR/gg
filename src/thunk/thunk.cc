#include "thunk.hh"



Thunk::Thunk( string outfile,  ThunkFunc thunkfunc, vector<InFileDescriptor> infiles)
    : outfile(outfile), thunkfunc(thunkfunc), infiles(infiles), order(0){

}

Thunk::~Thunk(){}

json Thunk::to_json(){
    json obj;

    obj["outfile"] = outfile;
    obj["function"] = thunkfunc.to_json();

    json j_infiles;
    for(auto it = infiles.begin(); it != infiles.end(); ++it){
        j_infiles.push_back(it->to_json());
    }

    obj["infiles"] = j_infiles;

    return obj.dump();
}