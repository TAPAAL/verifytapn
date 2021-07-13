# verifytapn

VerifyTAPN is a stand-alone verification engine for timed-arc Petri nets, developed for the verification tool TAPAAL. 

## Linux 

To compile verifytapn 

```

sudo apt install flex bison libboost-all-dev libsparsehash-dev cmake build-essential

mkdir build && cd build
cmake ..
make 
```

## Mac OS X 

Install homebrew

```
brew install cmake boost google-sparsehash flex bison 

mkdir build && cd build
cmake .. -DFLEX_EXECUTABLE=/usr/local/opt/flex/bin/flex -DBISON_EXECUTABLE=/usr/local/opt/bison/bin/bison
make

```


## Windows (Cross-compile)

To setup cross-compile: 

``` 
sudo apt install flex bison cmake build-essential mingw-w64-x86-64-dev mingw-w64-tools g++-mingw-w64-x86-64

TARGET=x86_64-w64-mingw32
ADDRM=64
MTUNE=generic
CORES=$(nproc || 1)
export CC=x86_64-w64-mingw32-gcc
export CXX=x86_64-w64-mingw32-g++
export PREFIX=$(mktemp -d -p .)
export PREFIX=$(realpath $PREFIX)
        
BOOST=boost_1_72_0
VER=${BOOST#boost_}
VER=${VER//_/.}
        
wget -nv "https://boostorg.jfrog.io/artifactory/main/release/$VER/source/$BOOST.tar.bz2"
tar xf $BOOST.tar.bz2
cd $BOOST
CC= CXX= ./bootstrap.sh --prefix="$PREFIX" --without-icu
cat > win$ADDRM-config.jam << EOF
using gcc : m : $CXX
        :
;
EOF
./b2 toolset=gcc-m target-os=windows cflags="-mtune=$MTUNE" cxxstd=17 cxxflags="-mtune=$MTUNE" address-model="$ADDRM" binary-format="pe" abi="ms" threading="multi" threadapi="win32" variant=release --user-config="win$ADDRM-config.jam" --prefix="$PREFIX" --without-mpi --without-python --without-coroutine --without-graph --without-graph_parallel --without-wave --without-context -sNO_BZIP2=1 -j$CORES install

cd ..
sudo apt install unzip
wget https://github.com/sparsehash/sparsehash/archive/sparsehash-2.0.4.zip
unzip sparsehash-2.0.4.zip
cd sparsehash-sparsehash-2.0.4
./configure CXXFLAGS=-std=c++11 --prefix="$PREFIX" --host win
make
make install
cd ..
``` 

To compile verifytapn: 

```
mkdir build-win && cd build-win
cmake .. -DCMAKE_TOOLCHAIN_FILE=../toolchain-x86_64-w64-mingw32.cmake -BOOST_ROOT=$PREFIX -DBoost_INCLUDE_DIR=$PREFIX/include
make 

``` 
