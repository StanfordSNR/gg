# Build LLVM Using `gg`

[https://llvm.org/](LLVM)

## Prerequisites

Follow [the instructions](https://github.com/StanfordSNR/gg/blob/master/README.md#build-directions) to compile and install the latest version of `gg` on your machine.

## Build

The time required to compile software is an evergreen frustration for software developers. Using model substitution, we implemented models for seven popular stages of a C or C++ software build pipeline: the preprocessor, compiler, assembler, linker, archiver, indexer, and strip. These allow us to automatically transform some software build processes (e.g., a Makefile or build.ninja file) into an expression in gg IR, which can then be executed with thousands-way parallelism on cloud-functions platforms to obtain the same results as if the build system hadbeen executed locally. You can read more on page ninth of [the paper](https://www.usenix.org/system/files/atc19-fouladi.pdf).

Let's see how you can speed up the build of LLVM using `gg`.

### Useful Links

[gg + CMake for building LLVM](https://github.com/StanfordSNR/gg/issues/22)

[![asciicast](https://asciinema.org/a/262982.svg)](https://asciinema.org/a/262982)

### Run

There is one simple entry point to run the demo:

```sh
./bin/run.sh <JOBS-COUNT>

./llvm-build/bin/llvm-tblgen --help
# USAGE: llvm-tblgen [options] <input file>
#
# OPTIONS:
#
# Color Options:
# --color        - Use colors in output (default=autodetect)
# ...
```
