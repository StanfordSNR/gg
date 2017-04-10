#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/wait.h>

const char *PREPROC_FLAG = "-E";
const char *COMPILER = "gcc";
const char *OUTFLAG = "-o";
const char *COMPILEFLAG = "-c";
const char PREPROC_SUFFIX = 'i';
bool verbose = false;
const char *COMPILE_COMMAND[] = {"gcc", "-g", "-O2", "-c", "-o", "",  "", '\0'};
const int OUTINDEX = 5;
const int SRCINDEX = OUTINDEX+1;



bool getPreprocArgs(int argc, char *argv[], char*preproc_args[], char object_file[]){
    bool postgcc = false; // track if you've seen the gcc command yet
    bool outflag = false; // track the -o outflag in gcc command
    bool isLinking = true;
    int object_index;
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
    preproc_args[argc+1] = '\0';
    return isLinking;
}

int main(int argc, char *argv[]){

    if (argc < 2 || strcmp(argv[1], COMPILER) != 0){
        std::cout << "This is a wrapper for gcc. It can only be called before gcc." << std::endl;
        exit(-1); // TODO: Replace with real error code
    }
    int status = 0;
    char object_file[50];

    char *preproc_args[argc + 2];
    bool isLinking = getPreprocArgs(argc, argv, preproc_args, object_file);

    int pid = fork();

    if(pid == 0) {  // child

        if (verbose) {
            for (int i = 1; i <= argc + 1; i++) {
                std::cout << preproc_args[i] << " ";
            }
            std::cout << std::endl;
        }
        if(!isLinking) {
            execvp(argv[1], (char *const *) &preproc_args[1]);
        }else{
            execvp(argv[1], (char *const *) &argv[1]);
        }
    }
    else{  // parent
        wait(&status);
        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
            std::cerr << "Process  (pid " << pid << ") failed" << std::endl;
            exit(1);
        }
    }

    char srcfile[50];
    strcpy(srcfile, object_file);
    srcfile[strlen(srcfile)-1] = PREPROC_SUFFIX;
    COMPILE_COMMAND[OUTINDEX] = object_file;
    COMPILE_COMMAND[SRCINDEX] = srcfile;

    pid = fork();

    if(pid == 0) {  // child

        if(isLinking){
            exit(0);
        }

        if (verbose) {
            for (int i = 0; i < 7; i++) {
                std::cout << COMPILE_COMMAND[i] << " ";
            }
            std::cout << std::endl;
        }

        execvp ("gcc", (char* const*)&COMPILE_COMMAND);
    }
    else{  // parent
        wait(&status);
        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
            std::cerr << "Process  (pid " << pid << ") failed" << std::endl;
            exit(1);
        }
    }

    if(!isLinking){
        std::stringstream ss;
        ss << "rm " << COMPILE_COMMAND[SRCINDEX];
        std::cout << ss.str() << std::endl;
        system(ss.str().c_str());
    }

}