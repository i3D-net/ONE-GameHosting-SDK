#!/bin/bash

cd ..
mkdir -p build

# -S and -B options are not supported on CMake versions below 1.13, so enter
# build folder instead and run `cmake ..``.
# cmake -S . -B ./build
cd build
CFLAGS=-m64
CXXFLAGS=-m64
cmake ..
cd ..

cmake --build ./build --config Release --target all