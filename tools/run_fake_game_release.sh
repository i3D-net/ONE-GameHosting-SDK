#!/bin/bash

echo running fake game release

cd ..

if [ -d "build/one/fake/game/debug" ]; then
    cd build/one/fake/game/debug
else
    cd build/one/fake/game
fi
./game
