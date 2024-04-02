#!/bin/bash
rm -rf $PWD/cmake-build-arm
cmake  -S $PWD -B $PWD/cmake-build-arm
cmake --build $PWD/cmake-build-arm
