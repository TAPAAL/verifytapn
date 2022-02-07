# verifytapn

VerifyTAPN is a stand-alone verification engine for timed-arc Petri nets, developed for the verification tool TAPAAL.

## Linux

To compile verifytapn

```

sudo apt install flex bison cmake build-essential

mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Prebuild
make
cmake .. -DCMAKE_BUILD_TYPE=Release
make
```

## Mac OS X

Install homebrew

```
brew install cmake flex bison

mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Prebuild -DFLEX_EXECUTABLE=/usr/local/opt/flex/bin/flex -DBISON_EXECUTABLE=/usr/local/opt/bison/bin/bison
make
cmake .. -DCMAKE_BUILD_TYPE=Release -DFLEX_EXECUTABLE=/usr/local/opt/flex/bin/flex -DBISON_EXECUTABLE=/usr/local/opt/bison/bin/bison
make
```

Alternative bison and flex locations:

```
cmake -DBISON_EXECUTABLE=/opt/homebrew/opt/bison/bin/bison -DFLEX_EXECUTABLE=/opt/homebrew/opt/flex/bin/flex ..
```

## Windows (Cross-compile)

To setup cross-compile:

```
sudo apt install flex bison cmake build-essential mingw-w64-x86-64-dev mingw-w64-tools g++-mingw-w64-x86-64
```


To compile verifytapn:

```
mkdir build-win && cd build-win
cmake .. -DCMAKE_BUILD_TYPE=Prebuild -DCMAKE_TOOLCHAIN_FILE=../toolchain-x86_64-w64-mingw32.cmake
make
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../toolchain-x86_64-w64-mingw32.cmake
make

```
