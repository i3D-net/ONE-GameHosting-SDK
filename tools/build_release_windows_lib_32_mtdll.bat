 cd ..
 mkdir build
 
 :: Toolchain version 15 - Visual studio 2017.
 cmake -G "Visual Studio 15" -S . -B ./build
 cmake --build ./build --config Release --target ALL_BUILD