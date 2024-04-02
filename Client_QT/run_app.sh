#!/bin/bash

file_name="MultimeterInterface"
file_path="$PWD/cmake-build-arm"

cd "$file_path"

if [ -x "$file_name" ]; then
    ./"$file_name"
else
    echo "Error: Executable file $file_name not found in directory $file_path"
    exit 1
fi
