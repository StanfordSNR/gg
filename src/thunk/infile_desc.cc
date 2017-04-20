#include "infile_desc.hh"

#include <iostream>

InFileDescriptor::InFileDescriptor( string filename ) 
    : hash(), filename(filename), order(0) {
    
}

InFileDescriptor::~InFileDescriptor(){}

Json::Value InFileDescriptor::to_json(){
    Json::Value j;
    j["test"] = "UNIMPLEMENTED";
    return j;
}