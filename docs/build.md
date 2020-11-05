# Build

Prebuilt binaries are currently not supplied. The integration code is very lightweight. Integrations have two options:

1. Copy the code in one/arcus directly into the game engine and build directly as part of the regular build.
2. Or build the repository with the following instructions, and then headers and binaries may be copied as needed. Binaries will be output into the build folder, e.g. build/one/arcus/release/one_arcus.lib.

Building the repository in either case is recommended as it will also create a fake agent executable to aid testing of your game server without deploying to the remote one platform.

## Requirements

In order to build the source you will need to ensure you have the following dependencies installed on your system.

1. A C++ compiler is needed.
    - Windows: [Visual Studio 2017, or build tools (without Editor)](https://visualstudio.microsoft.com/vs/older-downloads/)
    - Linux: GCC (7.5.0 tested)
2. [CMake 3.17.4](https://cmake.org/files/v3.17/). This is used to configure and build the project. See the top of CMakeLists.txt for other CMake versions tested, if you are using a different version already.

Optional
1. For documentation generation:
    - Windows:
        1. [doxygen](https://www.doxygen.nl/manual/install.html#install_bin_windows)
        2. [graphviz](https://graphviz.org/download/)
    - Ubuntu:
        1. `sudo apt-get update`
        2. `sudo apt-get install doxygen graphviz`

## Steps

Build scripts for both platforms can be run from the root:
- build_release_windows.bat
- build_release_linux.sh

There are also debug versions. Run the one for your platform and you should see build succeed and tests run successfully. Build output is put into the build folder.

That's it!

## Clean

Run either script in the root:
- clean_windows.bat
- clean_linux.sh

## IDE

To load the repository in Visual Studio, first build and then open the generated solution file located at build/OneGameHostingSDK.sln.

For working on the SDK in VSCode, IDE tips are located [here](docs/vscode.md).

## Build details

The `-S` and `-B` options require CMake 3.13 or newer. Older version must cd into the build folder and run `cmake ..`.

A subset of tests will run as part of the build process.

The doxygen documentation is built automatically if CMake is able to find the doxygen installed on your system.

## Testing

As noted above, short tests are run at the end of the cmake build, by default.

Some bash scripts in the root can be used to run longer tests:
- run_tests_debug_long.sh
- run_tests_debug_soak.sh

Use the `build/tests/debug/tests.exe -?` option for all commands.

