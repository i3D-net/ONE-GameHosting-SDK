 mkdir build
 :: Toolchain version 15 - Visual studio 2017.
 cmake -G "Visual Studio 15" -S . -B ./build -T host=x64 -A x64
 cmake --build ./build --config Debug --target ALL_BUILD