 cd ..
 mkdir build
 
 :: Toolchain version 15 - Visual studio 2017.
 cmake -G "Visual Studio 15" -S . -B ./build -T host=x86 -A win32
 cmake --build ./build --config Release --target ALL_BUILD