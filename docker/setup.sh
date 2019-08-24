# compile gg
./fetch-submodules.sh
./autogen.sh
./configure
make -j$(nproc)
make install

# init serverless functions
make ggfunctions
