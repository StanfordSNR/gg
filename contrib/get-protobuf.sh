set -xe
cd third_party
version=3.12.0
if [[ -a protobuf-${version} ]]; then
    echo "Protobuf already there"
else
    wget https://github.com/protocolbuffers/protobuf/releases/download/v${version}/protobuf-cpp-${version}.tar.gz
    tar xvf protobuf-cpp-${version}.tar.gz
    rm protobuf-cpp-${version}.tar.gz
    cd protobuf-${version}
    ./autogen.sh
    ./configure --prefix=$(pwd)/../install
    make -j$(nproc)
    make install
fi
