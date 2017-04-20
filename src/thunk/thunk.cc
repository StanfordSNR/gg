#include "thunk.hh"



Thunk::Thunk( string outfile,  ThunkFunc thunkfunc, vector<InFileDescriptor> infiles)
    : outfile(outfile), thunkfunc(thunkfunc), infiles(infiles), order(0){

}

Thunk::~Thunk(){}

Json::Value Thunk::to_json(){
    Json::Value obj;

    obj["outfile"] = outfile;
    obj["function"] = thunkfunc.to_json();

    Json::Value j_infiles;
    for(auto it = infiles.begin(); it != infiles.end(); ++it){
        j_infiles.append(it->to_json());
    }

    obj["infiles"] = j_infiles;

    // Json::StyledWriter styled;
    Json::FastWriter fast;
    return fast.write(obj);
}