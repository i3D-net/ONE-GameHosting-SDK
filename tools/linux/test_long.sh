#!/bin/bash

BIN_DIR="../../build/bin"
FILE_NAME="tests"

if [ -e "$BIN_DIR/$FILE_NAME" ]; then
    $BIN_DIR/$FILE_NAME long:*
else
    echo "No $FILE_NAME executable found in $BIN_DIR/"
    exit 1
fi
