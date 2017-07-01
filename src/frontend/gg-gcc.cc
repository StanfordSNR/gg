#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/wait.h>
#include <ctime>
#include <sys/time.h>
#include <chrono>
#include <vector>

#include "exception.hh"
#include "syscall.hh"
#include "traced_process.hh"
#include "thunk.hh"
#include "thunk_writer.hh"

using namespace std;
using namespace gg::thunk;

const char *PREPROC_FLAG = "-E";
const char *COMPILER = "gcc";
const char *OUTFLAG = "-o";
const char *COMPILEFLAG = "-c";
const char PREPROC_SUFFIX = 'i';
bool verbose = false; // TODO add to commandline args
const char *COMPILE_COMMAND[] = {"gcc", "-g", "-O2", "-c", "-o", "",  "", NULL};
const size_t COMMAND_LEN = 7;
const int OUTINDEX = 5;
const int SRCINDEX = OUTINDEX+1;
bool model_only = true;



bool getPreprocArgs(int argc, char *argv[], vector<char *>& preproc_args, char object_file[]){
  bool postgcc = false; // track if you've seen the gcc command yet
  bool outflag = false; // track the -o outflag in gcc command
  bool isLinking = true;
  int object_index = -1;
  for(int i=0; i< argc; i++){
    preproc_args[postgcc ? i+1 : i ] = argv[i];

    if (strcmp(argv[i], COMPILER) == 0){
      postgcc = true;
      preproc_args[i+1] = (char*)PREPROC_FLAG;
    }

    // we want to rename the object file into a *.i file
    if(strcmp(argv[i], OUTFLAG)==0){
      outflag = true;
    }

    if(strcmp(argv[i] + strlen(argv[i]) - 2, ".o") == 0 && outflag){
      strcpy(object_file, argv[i]);
      object_index = i;
      outflag = false;
    }

    if(strcmp(argv[i], COMPILEFLAG)==0){
      isLinking = false;
    }
  }
  if(!isLinking){
    preproc_args[object_index+1][strlen(argv[object_index])-1] = PREPROC_SUFFIX;

  }
  preproc_args[argc+1] = NULL;

  return isLinking;
}

void create_thunk( char *purefile, char *objectfile, const char** cmd ){
  string outfile( objectfile );
  vector<string> cmd_vec( cmd , cmd + COMMAND_LEN );
  Function Function( cmd_vec );
  InFile infile( purefile );
  Thunk thunk( outfile, Function, {infile});
  ThunkWriter::write_thunk( thunk );
}

int main(int argc, char *argv[]){

  if (argc < 2 || strcmp(argv[1], COMPILER) != 0){
    cout << "This is a wrapper for gcc. It can only be called before gcc." << endl;
    exit(-1); // TODO: Replace with real error code
  }
  int status = 0;
  char object_file[50];

  vector<char *> preproc_args( argc+2);
  bool isLinking = getPreprocArgs(argc, argv, preproc_args, object_file);


  chrono::high_resolution_clock::time_point t0 = chrono::high_resolution_clock::now();

  char** cmd;
  if(!isLinking) {
    cmd = &preproc_args[1];
  }else{
    cmd = &argv[1];
  }

  if ( verbose ) {
    for (char** x = cmd; *x != NULL; ++x){
      cout << *x << " ";
    }
    cout << endl;
  }

  fflush(stdout);
  int pid = fork();

  if(pid == 0) {  // child

    if (verbose) {
      for (int i = 1; i <= argc + 1; i++) {
        cerr << preproc_args[i] << " ";
      }
      cerr << endl;
    }

    if( !isLinking ) {
      execvp( argv[ 1 ], (char *const *) &preproc_args[ 1 ] );
    }else{
      execvp( argv[ 1 ], (char *const *) &argv[ 1 ] );
    }
  }
  else {  // parent
    wait(&status);
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
      std::cerr << "Preprocess process (pid " << pid << ") failed" << std::endl;
      exit(1);
    }
    chrono::high_resolution_clock::time_point t1 = chrono::high_resolution_clock::now();
    auto dif = chrono::duration_cast<chrono::microseconds>(t1 - t0).count();
    if (verbose) {
      if (!isLinking) {
        cout << endl << "PREPROCTIME " << dif << endl;
      } else {
        cout << endl << "LINKINGTIME " << dif << endl;
      }
    }
  }

  if( !isLinking ){ // If we linked above, nothing else to do

    char srcfile[ 50 ];
    strcpy( srcfile, object_file );
    srcfile[ strlen( srcfile ) -1 ] = PREPROC_SUFFIX;
    COMPILE_COMMAND[ OUTINDEX ] = object_file;
    COMPILE_COMMAND[ SRCINDEX ] = srcfile;

    chrono::high_resolution_clock::time_point t2 = chrono::high_resolution_clock::now();

    fflush(stdout);
    pid = fork();

    if(pid == 0) {  // child

      if( verbose ) {
        for( int i = 0; i < 7; i++ ) {
          cout << COMPILE_COMMAND[ i ] << " ";
        }
        cout << endl;
      }

      if( model_only ){
        create_thunk( srcfile, object_file, COMPILE_COMMAND );
      } else {
        execvp( "gcc" , ( char* const* )&COMPILE_COMMAND );
      }

    }
    else{  // parent
      wait(&status);
      if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        std::cerr << "Compile process  (pid " << pid << ") failed" << std::endl;
        exit(1);
      }
      chrono::high_resolution_clock::time_point t3 = chrono::high_resolution_clock::now();
      auto dif = chrono::duration_cast<chrono::microseconds>( t3 - t2 ).count();
      if(!isLinking && verbose){
        cout << "COMPILETIME " <<  dif << endl;
      }
      if(!isLinking){
        std::stringstream ss;
        ss << "rm " << COMPILE_COMMAND[SRCINDEX];
        cout << ss.str() << endl;
        int ret = system(ss.str().c_str());
        if (ret != 0){
          cerr << "Error occurred deleting file: " << ss.str() << endl;
        }
      }
    }
  }



}
