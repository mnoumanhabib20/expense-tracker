@echo off
title PF Cpp Project - Build and Run
cd /d "%~dp0"

if not exist "main.cpp" (
    echo [ERROR] main.cpp is folder me nahi mili. Is .bat file ko main.cpp wale folder me rakhein.
    pause
    exit /b 1
)

where g++ >nul 2>nul
if errorlevel 1 (
    echo [ERROR] g++ nahi mila. MinGW install karein aur PATH me add karein.
    pause
    exit /b 1
)

REM Agar purani main.exe chal rahi ho to usay band kar do
taskkill /f /im main.exe >nul 2>nul

echo Compiling main.cpp ...
g++ main.cpp -o main -static -mwindows -lgdi32 -luser32
if errorlevel 1 (
    echo.
    echo [ERROR] Compile fail ho gaya. Upar error dekhein.
    pause
    exit /b 1
)

start "" "%~dp0main.exe"
exit /b 0
