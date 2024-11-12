@echo off
set BIN_DIR=%~dp0..\..\build\bin\Release

if exist "%BIN_DIR%\game.exe" (
    %BIN_DIR%\game.exe
) else  (
    echo "No fake game executable found in %BIN_DIR%\."
    exit /b 1
)
