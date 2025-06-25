@echo off

call  C:\apps\msys\ucrt64\bin\g++.exe ^
     -g ^
    -std=c++20 ^
    -I include ^
    -L lib ^
    src/main.cpp src/glad.c ^
    -lglfw3dll ^
    -o out/main.exe