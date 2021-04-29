#!/bin/bash

echo running fake agent release

cd ..

if [ -d "build/one/fake/agent/release" ]; then
    cd build/one/fake/agent/release
else
    cd build/one/fake/agent
fi
./agent
