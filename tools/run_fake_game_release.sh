#!/bin/bash

echo running fake game release

cd ..

if [ -d "build/one/game/debug" ]; then
    cd build/one/game/debug
else
    cd build/one/game
fi
./game