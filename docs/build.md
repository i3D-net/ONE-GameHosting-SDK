# Build

Prebuilt binaries are currently not supplied. The integration code is very lightweight. Integrations have two options:

1. Copy the code in one/arcus directly into the game engine and build directly as part of the regular build.
2. Or build the repository with the following instructions, and then copy required headers and binaries. Binaries will be output into the build folder, e.g. the build/one/arcus/release/one_arcus.lib.

Building the repository in either case is recommended as it will also create a fake agent executable to aid testing of your game server without deploying to the remote ONE Platform.

## Requirements

In order to build the source you will need to ensure you have the following dependencies installed on your system.

1. A C++ compiler is needed.
    - Windows: [Visual Studio 2017, or build tools (without Editor)](https://visualstudio.microsoft.com/vs/older-downloads/)
    - Linux: GCC (7.5.0 tested)
2. [CMake 3.17.4](https://cmake.org/files/v3.17/). This is used to configure and build the project. See the top of CMakeLists.txt for other CMake versions tested, if you are using a different version already.
3. After the initial repository clone, make sure to initialize the git submodules with the following command:
```
git submodule update --init --recursive
```

Optional
1. For documentation generation:
    - Windows:
        1. [doxygen](https://www.doxygen.nl/manual/install.html#install_bin_windows)
        2. [graphviz](https://graphviz.org/download/)
    - Ubuntu:
        1. `sudo apt-get update`
        2. `sudo apt-get install doxygen graphviz`
2. For buiding Linux libraries on Windows:
    - [Docker](https://www.docker.com/products/docker-desktop)

## Steps

Build scripts for both platforms can be found in the tools folder. The build scripts naming follows the following convention:

`build_<debug_or_release>_<platform>_<shared_or_static_library>_<architecture>`
Windows will also have a trailing `mt` or `mtdll` for the code generation CRT configuration (Multi Threaded vs Multi Threaded DLL).

For example build_release_windows_32_dll_mt.bat will configure and build on windows a release, 32 bit, multi-threaded (MT option in MSVC) dll library.

Running a build script will build the source, run tests and output results to the build folder.

A clean is needed in most cases when building with different settings.

## Clean

Run either script in the root:
- clean_windows.bat
- clean_linux.sh

Cleaning is not required when changing between debug and release builds with other settings being equal.

## Automated build

__build_release_dlls.bat__ will build dll/so libraries for Windows and Linux, outputing them to a shared_lib_build folder in the root. Docker is required, see Requirements.

## IDE

To load the repository in Visual Studio, first build and then open the generated solution file located at build/OneGameHostingSDK.sln.

For working on the SDK in Visual Studio Code, IDE tips are located [here](docs/vscode.md).

## Documentation Build

The doxygen documentation is built automatically if CMake is able to find the doxygen installed on your system.

## Testing

As noted above, short tests are run at the end of the cmake build, by default.

Run the following bash scripts from within the tools folder to run longer tests:
- run_tests_debug_long.sh
- run_tests_debug_soak.sh

Use the `build/tests/debug/tests.exe -?` option for all commands.
