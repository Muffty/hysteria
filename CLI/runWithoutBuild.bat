@echo off

.\build\Release\HysteriaCLI.exe
if %errorlevel% neq 0 (
    echo ❌ CLI program crashed. See %LOGFILE%
    type %LOGFILE%
    pause
    exit /b 1
)
