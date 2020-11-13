#!/bin/bash

echo running fake agent debug
if [ -d "build/one/agent/debug" ]; then
    cd build/one/agent/debug
else
    cd build/one/agent
fi
./agent
