@echo off

mkdir "build"
g++ "src\test.cpp" "lib\lodepng\lodepng.cpp" "-Wall" "-Wextra" "-O3" "-o" "build\test"

IF ERRORLEVEL == 0 (
    .\build\test
)