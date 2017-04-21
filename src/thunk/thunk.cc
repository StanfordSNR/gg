#include "thunk.hh"



Thunk::Thunk( string outfile,  ThunkFunc thunkfunc, vector<InFileDescriptor> infiles)
    : outfile(outfile), thunkfunc(thunkfunc), infiles(infiles), order(0){

}

Thunk::~Thunk(){}

json::Object Thunk::to_json(){
    json::Object obj;

    obj["outfile"] = json::String(outfile);
    obj["function"] = thunkfunc.to_json();

    json::Array j_infiles;
    for(auto it = infiles.begin(); it != infiles.end(); ++it){
        j_infiles.Insert(it->to_json());
    }

    obj["infiles"] = j_infiles;

    return obj;
}