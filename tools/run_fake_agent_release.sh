#!/bin/bash

echo running fake agent release

cd ..

if [ -d "build/one/agent/release" ]; then
    cd build/one/agent/release
else
    cd build/one/agent
fi
./agent
