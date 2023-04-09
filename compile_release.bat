@echo off

mkdir "build"
g++ "src\main.cpp" "lib\lodepng\lodepng.cpp" "-Wall" "-Wextra" "-O3" "-o" "build\invisible_ink"

IF ERRORLEVEL == 0 (
  echo "build/invisible_ink was created"
)