@echo off
set BIN_DIR=%~dp0..\..\build\bin\Release

if exist "%BIN_DIR%\tests.exe" (
    rem Short test using exclude filters
    %BIN_DIR%\tests.exe ~[.] ~long:* ~soak:* ~soak-days:*
) else  (
    echo "No tests executable found in %BIN_DIR%\."
    exit /b 1
)
