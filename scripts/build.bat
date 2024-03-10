@echo off
g++ --version

if not exist build\ mkdir build
set params=-O3 -Wall -I src\include -std=c++17

echo Building shell.cpp
g++ %params% src\shell.cpp -o build\shell.exe

for %%f in (src\commands\*) do (
    if /i %%~xf equ .cpp (
        echo Compiling %%f to build\%%~nf.exe
        g++ %params% %%f -o build\%%~nf.exe
    )
)
