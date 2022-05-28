set -e
cd third_party
git clone --depth 1 -b OpenSSL_1_1_1g https://github.com/openssl/openssl.git
cd openssl
./config --prefix=$(pwd)/../install
make -j $(nproc)
make install -j $(nproc)
