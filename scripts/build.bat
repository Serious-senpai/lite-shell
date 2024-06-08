@echo off
setlocal enabledelayedexpansion

g++ --version

for %%f in ("%~dp0..") do set root=%%~ff
echo Got root of repository: %root%

if not exist %root%\build mkdir %root%\build
set before=-O3 -Wall -I %root%\extern\regex\include -I %root%\src\include -std=c++17
set after=-l wininet

if "%1"=="debug" (
    set before=-D DEBUG %before%
    set debug=true
    echo Building in debug mode
) else (
    set debug=false
)

set link=
for %%f in (%root%\src\include\*) do (
    if "%%~xf" == ".cpp" (
        echo Compiling %%f to %root%\build\%%~nf.o
        g++ -c %before% %%f %after% -o %root%\build\%%~nf.o
        set link=!link! %root%\build\%%~nf.o
        if !errorlevel! neq 0 exit /b !errorlevel!
    )
)

echo Compiling %root%\src\shell.cpp to %root%\build\shell.o
g++ -c %before% %root%\src\shell.cpp %after% -o %root%\build\shell.o
if %errorlevel% neq 0 exit /b %errorlevel%

echo Building %root%\build\shell.exe
g++ -o %root%\build\shell.exe %link% %root%\build\shell.o
if %errorlevel% neq 0 exit /b %errorlevel%

for %%f in (%root%\src\external\*) do (
    if "%%~xf" == ".cpp" (
        echo Compiling %%f to %root%\build\%%~nf.o
        g++ -c %before% %%f %after% -o %root%\build\%%~nf.o
        if !errorlevel! neq 0 exit /b !errorlevel!

        echo Building %root%\build\%%~nf.exe
        g++ -o %root%\build\%%~nf.exe %link% %root%\build\%%~nf.o
        if !errorlevel! neq 0 exit /b !errorlevel!
    )
)
