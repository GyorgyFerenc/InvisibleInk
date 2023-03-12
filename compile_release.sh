#!/bin/sh
mkdir build 2> /dev/null
g++ src/main.cpp ./lib/lodepng/lodepng.cpp -Wall -Wextra -O3 -o build/invisible_ink