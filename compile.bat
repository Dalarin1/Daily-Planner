@echo off

call  g++.exe ^
     -g ^
     -O3 ^
    -std=c++20 ^
    -I include ^
    -L lib ^
    src/main.cpp src/glad.c ^
    -lglfw3dll ^
    -lfreetype ^
    -o out/main.exe
