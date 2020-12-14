#!/bin/bash

echo running tests

cd ..

if [ -d "build/tests/debug" ]; then
    cd build/tests/debug
else
    cd build/tests
fi
./tests long:*
