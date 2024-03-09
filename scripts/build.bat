@echo off
if not exist build/ mkdir build
echo Building shell.cpp
g++ -O3 -Wall -std=c++17 src/shell.cpp -o build/shell.exe
