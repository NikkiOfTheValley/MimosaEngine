#!/bin/bash

if [[ "$1" == "-debug" ]]; then
   cmake -G "Unix Makefiles" -S "./" -B "build" -D CMAKE_BUILD_TYPE=Debug
else
   cmake -G "Unix Makefiles" -S "./" -B "build" -D CMAKE_BUILD_TYPE=Release
fi

cmake --build "build" -j $(nproc)