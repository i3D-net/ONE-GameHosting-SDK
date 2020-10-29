#!/bin/bash

echo running tests
cd build/tests/debug
./tests soak:*
