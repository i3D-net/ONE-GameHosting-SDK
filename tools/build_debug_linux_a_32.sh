#!/bin/bash
cmake -S ../ -B ../build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS=-m32 -DCMAKE_C_FLAGS=-m32
cmake --build ../build/ --target all