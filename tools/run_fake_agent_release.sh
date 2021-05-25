#!/bin/bash

echo running fake agent release

cd ..

if [ -d "build/one/fake/arcus/agent/release" ]; then
    cd build/one/fake/arcus/agent/release
else
    cd build/one/fake/arcus/agent
fi
./agent
