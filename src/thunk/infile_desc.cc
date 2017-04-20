#include "infile_desc.hh"

#include <iostream>

InFileDescriptor::InFileDescriptor( string filename ) 
    : hash(), filename(filename), order(0) {
    
}

InFileDescriptor::~InFileDescriptor(){}

json InFileDescriptor::to_json(){
    json j;
    j["test"] = "UNIMPLEMENTED";
    return j;
}