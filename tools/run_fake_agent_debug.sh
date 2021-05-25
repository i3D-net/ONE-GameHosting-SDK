#!/bin/bash

echo running fake agent debug

cd ..

if [ -d "build/one/fake/arcus/agent/debug" ]; then
    cd build/one/fake/arcus/agent/debug
else
    cd build/one/fake/arcus/agent
fi
./agent
