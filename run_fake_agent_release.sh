#!/bin/bash

echo running fake agent release
if [ -d "build/one/agent/release" ]; then
    cd build/one/agent/release
else
    cd build/one/agent
fi
./agent
