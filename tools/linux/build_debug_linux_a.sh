#!/bin/bash
cmake -S ../ -B ../build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS=-m64 -DCMAKE_C_FLAGS=-m64
cmake --build ../build/ --target all