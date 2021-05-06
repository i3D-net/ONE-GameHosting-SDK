#!/bin/bash

echo running fake game debug

cd ..

if [ -d "build/one/fake/game/debug" ]; then
    cd build/one/fake/game/debug
else
    cd build/one/fake/game
fi
./game
