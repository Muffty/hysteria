@echo off
if not exist build mkdir build
cd build
cmake ..
if %errorlevel% neq 0 (
    echo ❌ CMake configuration failed. See %LOGFILE%
    type %LOGFILE%
    pause
    exit /b 1
)

cmake --build . --config Release
if %errorlevel% neq 0 (
    echo ❌ Build failed. See %LOGFILE%
    type %LOGFILE%
    pause
    exit /b 1
)

.\Release\HysteriaCLI.exe
if %errorlevel% neq 0 (
    echo ❌ CLI program crashed. See %LOGFILE%
    type %LOGFILE%
    pause
    exit /b 1
)

cd ..
