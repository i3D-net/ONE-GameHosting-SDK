#!/bin/bash

cmake -S . -B ./build
cmake --build ./build --config Debug --target all