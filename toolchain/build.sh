#!/bin/bash

OPTIND=1
debug=0

while getopts "h?d" opt; do
    case "$opt" in
    h|\?)
        echo "Use -d flag to build debuggable version"
        exit 0
        ;;
    d)  debug=1
        ;;
    esac
done
shift "$((OPTIND-1))"

## Download all dependencies
linux_file="linux-4.10.14"
binutils_file="binutils-2.28"
libgg_file="libgg"
gcc_file="gcc-6.3.0"

if [ ! -d $linux_file ]
then
    curl https://cdn.kernel.org/pub/linux/kernel/v4.x/linux-4.10.14.tar.xz  | tar Jx
fi
if [ ! -d $binutils_file ]
then
    curl https://ftp.gnu.org/gnu/binutils/binutils-2.28.tar.gz | tar xz
fi
if [ ! -d $libgg_file ]
then
    git clone https://github.com/StanfordSNR/libgg.git
fi
if [ ! -d $gcc_file ]
then
    curl https://ftp.gnu.org/gnu/gcc/gcc-6.3.0/gcc-6.3.0.tar.gz | tar xz
fi

## Create musl standalone(static) compiler
## Custom Optimizations
if [ $debug -eq 1 ]
then 
    OPT='-Og'
else
    OPT='-Os -mtune=generic -fomit-frame-pointer'
fi
## Number of threads
WORKERS=
## Arch short designation (amd64 not recognized by musl)
ARCH='x86_64'
## Arch full designation; must end with -musl 
TARGET='x86_64-linux-musl'

## End of user defined variables
export PREFIX="`pwd`/build-$TARGET"
if [ $debug -eq 1 ]
then
    export CFLAGS="$OPT -fno-fast-math -w -g" # -s"
    export CXXFLAGS="$OPT -fno-fast-math -w -g" # -s"
else
    export CFLAGS="$OPT -fno-fast-math -w -g -s"
    export CXXFLAGS="$OPT -fno-fast-math -w -g" # -s"
fi
export PATH="$PREFIX/bin:$PATH"

## Abort on error
set -e

## Clean if exists
rm -rf "$PREFIX"
mkdir -p "$PREFIX"

echo Working...

## Fix path to usr inside $PREFIX
cd "$PREFIX"
ln -nfs . usr
cd ..

## Build temp libgg
rm -rf build-libgg
mkdir build-libgg
cd build-libgg
CROSS_COMPILE=" " ../libgg/configure --prefix="$PREFIX" --target="$ARCH" --disable-shared 1>/dev/null
make -j$WORKERS 1>/dev/null
make install 1>/dev/null
cd ..
rm -rf build-libgg
echo "1/7 libgg done."

## Build temp binutils
rm -rf build-binutils
mkdir build-binutils
cd build-binutils
../binutils*/configure --prefix="$PREFIX" --target="$TARGET" --disable-bootstrap --disable-werror 1>/dev/null
make -j$WORKERS 1>/dev/null
make install 1>/dev/null
cd ..
rm -rf build-binutils
echo "2/7 BINUTILS done."

## Build temp gcc
rm -rf build-gcc
mkdir build-gcc
cd build-gcc
../gcc*/configure --prefix="$PREFIX" --target="$TARGET" --with-sysroot="$PREFIX" --disable-multilib --disable-bootstrap --disable-werror --disable-shared --enable-languages=c,c++ --disable-libsanitizer --disable-nls 1>/dev/null
make -j$WORKERS 1>/dev/null
make install 1>/dev/null
cd ..
rm -rf build-gcc
echo "3/7 GCC done."

## Fix paths
export CC="$TARGET-gcc"
export CXX="$TARGET-g++"

export PREFIX="`pwd`/$TARGET"
export CFLAGS="$CFLAGS -static --sysroot="$PREFIX" -g"
export CXXFLAGS="$CXXFLAGS -static --sysroot="$PREFIX" -g"

## Clean existing
rm -rf "$PREFIX"

## Create linux headers
cd linux*
make ARCH="$ARCH" INSTALL_HDR_PATH="$PREFIX" headers_install 1>/dev/null
make clean 1>/dev/null
cd ..
echo "4/7 LINUX headers done."

## Fix usr path
cd "$PREFIX"
ln -nfs . usr
cd ..

## Build final libgg
rm -rf build-libgg
mkdir build-libgg
cd build-libgg
CROSS_COMPILE="$TARGET-" ../libgg/configure --prefix="$PREFIX" --target="$ARCH" --disable-shared --syslibdir="$PREFIX/lib" 1>/dev/null
make -j$WORKERS 1>/dev/null
make install 1>/dev/null
cd ..
rm -rf build-libgg
echo "5/7 libgg done."

## Build final binutils
rm -rf build-binutils
mkdir build-binutils
cd build-binutils
../binutils*/configure --prefix="$PREFIX" --target="$TARGET" --disable-bootstrap --disable-werror 1>/dev/null
make tooldir="$PREFIX" -j$WORKERS 1>/dev/null
make tooldir="$PREFIX" install 1>/dev/null
cd ..
rm -rf build-binutils
echo "6/7 BINUTILS done."

## Build final gcc
rm -rf build-gcc
mkdir build-gcc
cd build-gcc
../gcc*/configure --prefix="$PREFIX" --target="$TARGET" --with-sysroot="$PREFIX" --disable-multilib --disable-bootstrap --disable-werror --enable-languages=c,c++ --disable-libsanitizer --disable-shared --libexecdir="$PREFIX/lib" --disable-nls 1>/dev/null
make -j$WORKERS 1>/dev/null
make install 1>/dev/null
cd ..
rm -rf build-gcc

## Move gcc include/lib to correct directories
cd "$PREFIX/$TARGET"
cp -rf include ..
cp -rf lib ..
cp -rf lib64/. ../lib
rm -rf "$PREFIX/$TARGET"

echo "7/7 GCC done."

rm -rf "$PREFIX/../build-$TARGET"

echo All done.
