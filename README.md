# gg [![Build Status](https://travis-ci.org/StanfordSNR/gg.svg?branch=master)](https://travis-ci.org/StanfordSNR/gg)

[![xkcd.com/303/](https://s3-us-west-2.amazonaws.com/stanfordsnr/gg-xkcd.jpg)](https://xkcd.com/303/)


## Research Paper

"[From Laptop to Lambda: Outsourcing Everyday Jobs to Thousands of Transient Functional Containers](https://www.usenix.org/system/files/atc19-fouladi.pdf)," in 2019 USENIX Annual Technical Conference (USENIX ATC '19).

## Demo

Building FFmpeg with `gg` on AWS Lambda

[![asciicast](https://asciinema.org/a/257545.svg)](https://asciinema.org/a/257545)

## Tech Video

`gg` was presented at 2019 USENIX Annual Technical Conference (USENIX ATC '19). The talk was given on July 11, 2019.

[![Alt text](https://img.youtube.com/vi/VVWVN6Czji4/0.jpg)](https://www.youtube.com/watch?v=VVWVN6Czji4)


## Build directions

**Notice: If you are Docker adept, follow [this instruction](https://github.com/StanfordSNR/gg/blob/master/docker/README.md).**

To build `gg` you need the following packages:

- `gcc` >= 7.0
- `protobuf-compiler`, `libprotobuf-dev` >= 3.0
- `libcrypto++-dev` >= 5.6.3
- `python3`
- `libcap-dev`
- `libncurses5-dev`
- `libboost-dev`
- `libssl-dev`
- `autopoint`
- `help2man`
- `texinfo`
- `automake`
- `libtool`
- `pkg-config`
- `libhiredis-dev`
- `python3-boto3`

You can install this dependencies in Ubuntu (17.04 or newer) by running:

```
sudo apt-get install gcc-7 g++-7 protobuf-compiler libprotobuf-dev \
                     libcrypto++-dev libcap-dev \
                     libncurses5-dev libboost-dev libssl-dev autopoint help2man \
                     libhiredis-dev texinfo automake libtool pkg-config python3-boto3
```

To build `gg`, run the following commands:

```
./fetch-submodules.sh
./autogen.sh
./configure
make -j$(nproc)
sudo make install
```

## Usage

### Environment Variables

To use `gg`, the following environment variables *must* be set:

- `GG_MODELPATH` => *absolute path* to `<gg-source-dir>/src/models/wrappers`.
- `GG_STORAGE_URI` =>
  - **S3**: `s3://<bucket-name>/?region=<bucket-region>`
  - **Redis**: `redis://<username>:<password>@<host>[:<port>]`
- `GG_LAMBDA_ROLE` => the role that will be assigned to the executed Lambda.
functions. Must have *AmazonS3FullAccess* and *AWSLambdaBasicExecutionRole*
permissions.
- `AWS_ACCESS_KEY_ID`, `AWS_SECRET_ACCESS_KEY` => your AWS access key
- `AWS_REGION` => your AWS region, where the functions are installed

### Installing the Functions

After setting the environment variables, you need to install `gg` functions on
AWS Lambda. To do so:

~~~
cd src/remote/
make ggfunctions
~~~

### Example

To build [`mosh`](https://github.com/mobile-shell/mosh) using `gg`, first we
checkout the mosh source code:

~~~
git clone https://github.com/mobile-shell/mosh
~~~

Make sure that you have all the dependencies for building `mosh`. In Ubuntu,
you can run:

~~~
sudo apt-get install -y automake libtool g++ protobuf-compiler libprotobuf-dev \
                        libboost-dev libutempter-dev libncurses5-dev \
                        zlib1g-dev libio-pty-perl libssl-dev pkg-config
~~~

Inside the `mosh` directory, first you need to prepare `mosh` to build:

~~~
./autogen.sh
./configure
~~~

Then,

~~~
gg init # creates .gg directory
gg infer make -j$(nproc) # creates the thunks
~~~

Now, to actually compile `mosh-server` on AWS Lambda with 100-way parallelism,
you can execute (it's important that `--jobs` comes before `--engine`):

~~~
gg force --jobs 100 --engine lambda src/frontend/mosh-server
~~~
