# Crosscomping all release libraries for windows and linux

For Windows machines, there's an option to build both Windows and Linux libraries using build_release_dlls.bat script.
This script creates libraries for both systems and both architectures (x86 and x64).
It does the following steps:

1. Creates the folder structure for the build (root folder called shared_lib_build).
2. Creates DLL files using build_release_windows_dll_32_mt.bat and build_release_windows_dll_64_mt.bat.
3. Builds the Docker image:
    3.1. Creates the internal folder structure.
    3.2. Builds SO files using build_release_linux_so_32.sh and build_release_linux_so_64.sh.
4. Creates a temporary Docker container.
5. Copies SO files from the container file system into the shared_lib_build folder.
6. Removes the temporary container.
