#!/bin/bash

BIN_DIR="../../build/bin"
FILE_NAME="game"

if [ -e "$BIN_DIR/$FILE_NAME" ]; then
    $BIN_DIR/$FILE_NAME
else
    echo "No $FILE_NAME executable found in $BIN_DIR/"
    exit 1
fi