@echo off
setlocal EnableDelayedExpansion

set BUILD_DIR=%~dp0..\..\build

rmdir /s /q %BUILD_DIR%

endlocal
exit /b !errorlevel!