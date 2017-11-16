# gg [![Build Status](https://travis-ci.org/StanfordSNR/gg.svg?branch=master)](https://travis-ci.org/StanfordSNR/gg)

## Build directions

To build `gg` you need the following packages:

- `gcc` >= 7.0
- `protobuf-compiler`, `libprotobuf-dev` >= 3.0
- `libcrypto++-dev` >= 5.6.3
- `python3`
- `libcap-dev`
- `libcajun-dev`
- `libkeyutils-dev`
- `libncurses5-dev`
- `libboost-dev`
- `libssl-dev`
- `autopoint`
- `help2man`
- `texinfo`
- `automake`
- `libtool`
- `pkg-config`

You can install this dependencies in Ubuntu (17.04 or newer) by running:

```
sudo apt-get install gcc-7 g++-7 protobuf-compiler libprotobuf-dev \
                     libcrypto++-dev libcap-dev libcajun-dev libkeyutils-dev \
                     libncurses5-dev libboost-dev libssl-dev autopoint help2man \
                     texinfo automake libtool pkg-config
```

To build `gg`, run the following commands:

```
./fetch-toolchain.sh
./autogen.sh
./configure
make -j$(nproc)
sudo make install
```

## Usage

To use `gg`, the following environment variables must be set:

- `GG_MODELPATH` => absolute path to `<gg-source-dir>/src/models/wrappers`
- `GG_S3_BUCKET` => S3 bucket that is going to be used for remote execution
- `GG_S3_REGION` => the region in which that S3 bucket lives
- `GG_LAMBDA_ROLE` => the role that will be assigned to the executed Lambda
functions. Must have *AmazonS3FullAccess* and *AWSLambdaBasicExecutionRole*
permissions.
- `WS_ACCESS_KEY_ID`, `AWS_SECRET_ACCESS_KEY` => your AWS access key

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

Inside the `mosh` directory:

~~~
gg-init # create .gg directory
gg-infer make -j$(nproc) # creates the thunks
~~~

Now, to actually compile `mosh-server` on AWS Lambda with 100-way parallelism,
you can execute:

~~~
GG_LAMBDA=1 GG_MAXJOBS=100 gg-force --status src/frontend/mosh-server
~~~
