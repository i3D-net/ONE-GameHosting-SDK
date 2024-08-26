@echo off
setlocal EnableDelayedExpansion

set SRC_DIR=%~dp0..
set BUILD_DIR=%~dp0..\build
set BUILD_OPTIONS=host=x64 -A x64 -DSHARED_ARCUS_LIB=ON
set BUILD_CONFIG=Debug

cmake -S %SRC_DIR% -B %BUILD_DIR% -T %BUILD_OPTIONS%
if !errorlevel! neq 0 (
    echo CMake configuration failed.
    endlocal
    exit /b 1
)
cmake --build %BUILD_DIR% --config %BUILD_CONFIG% --target ALL_BUILD
if !errorlevel! neq 0 (
    echo Build failed.
    endlocal
    exit /b 1
)

echo Build succeeded.
endlocal
exit /b 0