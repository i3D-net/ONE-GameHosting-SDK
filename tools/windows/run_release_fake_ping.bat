@echo off
set BIN_DIR=%~dp0..\..\build\bin\Release

if exist "%BIN_DIR%\ping.exe" (
    %BIN_DIR%\ping.exe
) else  (
    echo "No fake ping executable found in %BIN_DIR%\."
    exit /b 1
)
