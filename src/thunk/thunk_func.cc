#include "thunk_func.hh"

#include <iostream>

ThunkFunc::ThunkFunc( vector<string> cmd ) : args(){
    args = cmd;
    cout << "Command is :" << endl;
    for(auto it = cmd.begin(); it != cmd.end(); ++it){
        cout << *it << " ";
    }
    cout << endl;
}

ThunkFunc::~ThunkFunc(){}

Json::Value ThunkFunc::to_json() {
    Json::Value j;
    j["test"] = "UNIMPLEMENTED";
    return j;
}
