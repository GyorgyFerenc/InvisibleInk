#!/bin/sh
mkdir build 2> /dev/null
g++ src/test.cpp ./lib/lodepng/lodepng.cpp -Wall -Wextra -O3 -o build/test

if [ $? -eq 0 ]; then
    ./build/test
fi