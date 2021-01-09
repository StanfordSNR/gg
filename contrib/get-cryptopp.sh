set -e
cd third_party
git clone --depth 1 -b CRYPTOPP_8_2_0 https://github.com/weidai11/cryptopp.git
cd cryptopp
make libcryptopp.a -j $(nproc)
make install -j $(nproc) PREFIX=../install
