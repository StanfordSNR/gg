#include "infile_desc.hh"

#include <iostream>

InFileDescriptor::InFileDescriptor( string filename ) 
    : hash(), filename(filename), order(0) {
    
}

InFileDescriptor::~InFileDescriptor(){}

json::Object InFileDescriptor::to_json(){
    json::Object j;
    j["test"] = json::String("UNIMPLEMENTED");
    return j;
}