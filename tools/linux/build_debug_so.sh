#!/bin/bash
cmake -S ../../ -B ../../build -DCMAKE_BUILD_TYPE=Debug -DSHARED_ARCUS_LIB=ON
cmake --build ../../build/ --target all