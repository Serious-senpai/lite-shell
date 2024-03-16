@echo off
g++ --version

for %%f in ("%~dp0..") do set root=%%~ff
echo Got root of repository: %root%

if not exist %root%\build mkdir %root%\build
set params=-O3 -Wall -I %root%\src\include -std=c++17

echo Building %root%\src\shell.cpp to %root%\build\shell.exe
g++ %params% %root%\src\shell.cpp -o %root%\build\shell.exe
if %errorlevel% neq 0 exit /b %errorlevel%

for %%f in (%root%\src\external\*) do (
    if /i %%~xf equ .cpp (
        echo Building %%f to %root%\build\%%~nf.exe
        g++ %params% %%f -o %root%\build\%%~nf.exe
        if %errorlevel% neq 0 exit /b %errorlevel%
    )
)
