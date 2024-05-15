@echo off
setlocal enabledelayedexpansion

g++ --version

for %%f in ("%~dp0..") do set root=%%~ff
echo Got root of repository: %root%

if not exist %root%\build mkdir %root%\build
set params=-O3 -Wall -I %root%\extern\regex\include -I %root%\src\include -std=c++17

if "%1"=="debug" (
    set params=-D DEBUG %params%
    set debug=true
    echo Building in debug mode
) else (
    set debug=false
)

if "%1"=="assembly" (
    set params=-S -fverbose-asm %params%
    set assembly=true
    echo Compiling to assembly
) else (
    set assembly=false
)

if "%assembly%" == "true" (
    set extension=asm
) else (
    set extension=exe
)

echo Building %root%\src\shell.cpp to %root%\build\shell.%extension%
g++ %params% %root%\src\shell.cpp -o %root%\build\shell.%extension%
if %errorlevel% neq 0 exit /b %errorlevel%

for %%f in (%root%\src\external\*) do (
    if /i %%~xf equ .cpp (
        echo Building %%f to %root%\build\%%~nf.%extension%
        g++ %params% %%f -o %root%\build\%%~nf.%extension%
        if !errorlevel! neq 0 exit /b !errorlevel!
    )
)
