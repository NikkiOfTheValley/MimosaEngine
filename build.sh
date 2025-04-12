#!/bin/bash

cmake -G "Unix Makefiles" -S "./" -B "build"
cmake --build "build" -j $(nproc)