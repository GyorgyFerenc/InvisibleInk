#!/bin/sh
mkdir build
g++ src/main.cpp ./lib/lodepng/lodepng.cpp -Wall -Wextra -O3 -o build/invisible_ink