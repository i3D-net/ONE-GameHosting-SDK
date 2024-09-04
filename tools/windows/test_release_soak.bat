@echo off
set BIN_DIR=%~dp0..\..\build\bin\Release

if exist "%BIN_DIR%\tests.exe" (
    %BIN_DIR%\tests.exe soak:*
) else  (
    echo "No tests executable found in %BIN_DIR%\."
    exit /b 1
)
