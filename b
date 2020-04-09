#! /usr/bin/env bash

GENERATOR="Unix Makefiles"
#GENERATOR=Xcode

mkdir -p build
cd build
cmake .. -G "$GENERATOR"

