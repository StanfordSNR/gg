# ggSDK
```ggSDK``` is a Python SDK for interfacing with [```gg```](https://github.com/StanfordSNR/gg). It allows applications developers to create computation graphs and pipelines in native Python code that are eventually executable on ```gg```.

The outline of this README is as follows:
- ```ggSDK``` API
- Setting up ```ggSDK```
- Examples

Note: In this README, ```gg``` refers to the execution platform while GG refers to the ```ggSDK``` class.

## ggSDK API
### GG Class
```GG(cleanenv=True)```: Class Constructor
- **cleanenv**: Setting this to True (default) will remove the reductions and remote directories from .gg, which is the local directory used by gg to keep track of thunks and their reductions. This will allow ```gg``` to perform a “fresh” experiment run each time it runs. Setting this to False will maintain previous runs, which means that gg will likely not have to do any new thunk executions when rerun.

```clean_env(deepClean=False)```: Function to clean gg environment.
- **deepClean**: Only remove the reductions and remote directories. User can call this method with deepClean=True to remove all directories from .gg and start the ```gg``` environment from scratch.

```create_thunks(inputs)```: Creates all thunks recursively, starting from *inputs*. Does not execute them, thus allowing for the user to manage thunks separately
- **inputs**: one or more thunks to be starting point for thunk creations. For multiple thunks, pass in as a list. Function call will block until execution is completed.

Users almost always will only need to call ```create_thunks```.

### GGThunk Class
```GGThunk(exe, envars=[], outname='', exe_args=[], args_infiles=True)```: Class Constructor
- **exe**: Name of binary that will be run when the thunk is forced by ```gg```. Currently, this function must be a statically linked binary.
- **envars**: List of environment variables that should be set by gg to execute this thunk’s function. Defaults to empty. If there are no environment variables that need to be set, this can be left empty.
- **outname**: Name of output file. If no output name is given, ```gg``` will create one. Important: if your program produces an output file, it must have the same name as this outname, since ```gg``` will search for a file with this name upon completion of this thunk’s execution. It will also be used as an infile name into thunks that reference this thunk. Thus, it is best to pass a name for outname.
- **exe_args**: Executable arguments (such as flags, input files, output files, etc.). Passed in as a list of arguments. Defaults to an empty list. If there are no executable arguments that need to be set, this can be left empty.
- **args_infiles**: By default, ```gg``` will attempt to take all exe_args and turn them into infiles. This is especially useful if the executable’s arguments are all input files/data with no flags. However, for programs that mix flags with input files, ```ggSDK``` will not be able to differentiate between the two. Thus, if your exe_args are a mix of flags with input files, or if you prefer to explicitly pass in all infiles, set this parameter to be False.

```add_infile(all_inf, if_type='INVALID')```: Function to add an infile once the thunk is created.
- **all_inf**: one or more infiles to be added as dependencies. For multiple infiles, pass in as a list. Infiles can be a) the name of a file, b) the name of an executable, and/or c) a GGThunk object (thus creating a graph/pipeline). Infile types can be mixed within the all_inf list.
- **if_type**: infile type. This parameter is optional, and can be left as INVALID (the default) since ```ggSDK``` will automatically infer the type (which is especially useful when mixing different infile types).

```print_thunk()```: Function to print out the thunk in json format.

Users almost always will only need to call ```add_infile```.

## Setting up ggSDK
- Ensure ```gg``` is installed by cloning its project repository and following the installation instructions.
Once ```gg``` is installed, no further action needs to be performed to make it work with ```ggSDK```.

- ```ggSDK``` requires a few Python libraries that may not be installed on your machine: ```numpy```, ```futures```, and ```python_magic```. To install these two using pip, you can run the command:
```sudo pip install numpy futures python_magic```

- To use ```ggSDK```, simply add the following line to the top of your python script:
```from gg_sdk import GG, GGThunk```

## Examples
### Excamera
```
cd excamera-example
./fetch-deps.sh
./excam_ex.py <start> <end> <batch-size> <cq-level>

Example: ./excam_ex.py 0 4 2 32
```
Further information about the [Excamera](https://www.usenix.org/conference/nsdi17/technical-sessions/presentation/fouladi) [project](https://github.com/excamera).

### Video Decoding + Image Recognition
```
cd viddec-example
./fetch-deps.sh
./ffmpeg_gg.py 
```
