@echo off
set BIN_DIR=%~dp0..\..\build\bin\Debug

if exist "%BIN_DIR%\agent.exe" (
    %BIN_DIR%\agent.exe
) else  (
    echo "No fake agent executable found in %BIN_DIR%\."
    exit /b 1
)
