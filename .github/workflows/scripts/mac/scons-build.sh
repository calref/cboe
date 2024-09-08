#!/bin/sh -ve

export CC="$(brew --prefix llvm)/bin/clang"
export CXX="$(brew --prefix llvm)/bin/clang++"
export SDKROOT="$(xcrun --show-sdk-path)"

scons CXXFLAGS="-I/usr/local/opt/zlib/include" LINKFLAGS="-L/usr/local/opt/zlib/lib" $@
