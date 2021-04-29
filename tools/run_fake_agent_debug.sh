#!/bin/bash

echo running fake agent debug

cd ..

if [ -d "build/one/fake/agent/debug" ]; then
    cd build/one/fake/agent/debug
else
    cd build/one/fake/agent
fi
./agent
