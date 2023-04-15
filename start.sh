#!/bin/sh

export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:$PWD/3rd_parties/darknet
./build/birdeos
