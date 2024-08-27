#!/bin/bash
cmake -S ../ -B ../build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS=-m32 -DCMAKE_C_FLAGS=-m32 -DSHARED_ARCUS_LIB=ON
cmake --build ../build/ --target all