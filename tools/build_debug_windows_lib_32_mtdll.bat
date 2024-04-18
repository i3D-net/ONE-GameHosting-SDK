cd ..
mkdir build
:: Toolchain version 16 - Visual studio 2019.
cmake -G "Visual Studio 16 2019" -S . -B ./build -T host=x86 -A win32
if %errorlevel% neq 0 (
    :: fallback: Toolchain version 15 - Visual studio 2017.
    cmake -G "Visual Studio 15 2017" -S . -B ./build -T host=x86 -A win32
)
cmake --build ./build --config Debug --target ALL_BUILD