#!/bin/bash

echo running fake game release

cd ..

if [ -d "build/one/fake/arcus/game/debug" ]; then
    cd build/one/fake/arcus/game/debug
else
    cd build/one/fake/arcus/game
fi
./game
