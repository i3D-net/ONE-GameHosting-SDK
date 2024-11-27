# Build guide for windows

As the integration code is very lightweight, prebuilt binaries are not supplied with the SDK. There are two integration options:

1. Copy the source code from one/arcus or one/ping directly into the game-server or game-client and compile them as part of a project build.

2. Build the complete repository first using the instructions below, and then copy required headers and libraries. All binaries are placed into the build folder, e.g. `\build\lib\Release\one_arcus.lib` or `\build\bin\Debug\tests.exe`.

Building the repository is recommended in either case, as the build will also create a fake agent executable. The fake agent can be used to test your game server before deploying it to the remote ONE Platform. For more information about the fake agent see [/one/fake](/one/fake/readme.md)

## Build requirements

### Build tools

- MSVC (Microsoft Visual C++ compiler)
- cmake
- git

The following Windows configurations are tested and supported:

- Windows 10 (VS2019)
  - [Visual Studio Build Tools 2019](https://aka.ms/vs/16/release/vs_buildtools.exe) or Visual Studio 2019
    with: MSVC 142 (v14.29) compiler,  Windows 10 SDK (10.0.10941.0)
  - [cmake 3.17.4 or higher](https://cmake.org/download/)
  - [git](https://git-scm.com/download/win)

- Windows 10 (VS2022)
  - [Visual Studio Build Tools 2022](https://aka.ms/vs/17/release/vs_buildtools.exe) or Visual Studio 2022
    with: MSVC 143 (v14.41) compiler,  Windows 11 SDK (10.0.22621.0)
  - [cmake 3.23 or higher](https://cmake.org/download/)
  - [git](https://git-scm.com/download/win)

### Optional build tools

For automatic documentation generation:

- [doxygen](https://www.doxygen.nl/manual/install.html#install_bin_windows)
- [graphviz](https://graphviz.org/download/)

For building linux libraries on windows (more info [below](#automated-build))

- [Docker](https://www.docker.com/products/docker-desktop)

### Preparation

After the initial repository clone, make sure to initialize the git submodules.

```git-bash
git submodule update --init --recursive
```

## Build

Build scripts can be found in the `\tools\windows` folder. Scripts are provided with different build options, according to the following naming convention:

`build_<debug_or_release>_<static_or_shared_library>_<mt_or_mtdll>.bat`

- Debug or release build:
`_debug or _release` options specify whether to build the SDK in debug or release mode.

- Static or dynamic lLibrary:
`_lib or _dll` options specify whether to build the SDK as a static library (`.lib`) or a dynamic library (`.dll`).

- MultiThreaded or MultiThreaded DLL:
`_mt or _mtdll` options specify whether to use the MSVC Multi-Threaded runtime library with static linking or the MSVC Multi-Threaded DLL runtime library with dynamic linking.

Example: `\tools\windows\build_debug_lib_mtdll.bat`
Builds the SDK as a static library with dynamic linking of the MSVC runtime in debug mode.

## Clean

A clean of the build folder is needed before switching to a script with different build settings. The clean script `clean.bat` can be also found in and should be run from the `\tools\windows` folder.

## Test

Several tests-cases and test-collections are available after a successful build of the SDK as a static library. Test scripts are provided for the three most common used collections of tests and are located in the  `\tools\windows` folder.

- short test
`test_release_short.bat` or `test_debug_short.bat`

- long test (continuous test maintaining connection)
`test_release_long.bat` or `test_debug_long.bat`

- soak test (very high usage over a very long period on multiple threads)
`test_release_soak.bat` or `test_debug_soak.bat`

Its also possible to run single tests or other test groups. After compilation the `tests.exe` program is available in either the `\build\bin\Release` or `\build\bin\Debug` directory.

`tests.exe -?` displays help

`tests.exe -l` lists all test cases

`test.exe hello` runs a single test case (e.g. hello test)

## Alternative build methods

### Documentation build

The Doxygen source generated documentation is built automatically from every build script if CMake is able to find an installed version of Doxygen on your system.

### Automated build

When docker is installed it is possible to cross-compiling and build the most common used libraries for both windows and linux, using the script `build_release_all.bat` from the `/tools/all/` folder.

This script will build these  *release* versions:

- a windows dynamically linked library (.dll) with Multi-Threaded DLL runtime
- a linux shared object / dynamically linked library (.so)

Libraries will be placed in the `\shared_lib_build` folder in the root of the repository.

### Build from IDE

#### Visual Studio

Perform an initial build using the build script and then open the generated solution file `OneGameHostingSDK.sln` from the `\build` folder.

#### Visual Studio Code

For working on the SDK in Visual Studio Code, see our [vscode](vscode.md) documentation.
