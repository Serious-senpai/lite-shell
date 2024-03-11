@echo off
g++ --version

if not exist build\ mkdir build
set params=-O3 -Wall -I src\include -std=c++17

echo Building shell.cpp
g++ %params% src\shell.cpp -o build\shell.exe

for %%f in (src\commands\*) do (
    if /i %%~xf equ .hpp (
        echo Building standalone.exe to %%~nf.exe
        g++ %params% -D COMMAND_NAME=\"%%~nf\" src\standalone.cpp -o build\%%~nf.exe
    )
)
