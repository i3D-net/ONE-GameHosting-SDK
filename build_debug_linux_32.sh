#!/bin/bash

mkdir -p build
# -S and -B options are not supported on CMake versions below 1.13, so enter
# build folder instead and run `cmake ..``.
# cmake -S . -B ./build
cd build
CFLAGS=-m32 CXXFLAGS=-m32
cmake ..
cd ..

cmake --build ./build --config Debug --target all