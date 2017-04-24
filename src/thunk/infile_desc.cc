#include "infile_desc.hh"

#include <iostream>

using namespace std;

InFileDescriptor::InFileDescriptor( string filename ) 
    : hash_(), filename_(filename), order_(0) {
    
}

InFileDescriptor::~InFileDescriptor(){}

json::Object InFileDescriptor::to_json(){
    json::Object j;
    j["test"] = json::String("UNIMPLEMENTED");
    return j;
}