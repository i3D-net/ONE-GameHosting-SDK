# Build guide

Prebuilt binaries are currently not supplied. The integration code is very lightweight. Integrations have two options:

1. Copy the code in one/arcus directly into the game engine and build directly as part of a project build.
2. Or build the complete repository using the following instructions, and then copy required headers and binaries.
   Binaries will be output into the build folder, e.g. build/bin/Release/tests.exe or build/lib/Release/one_arcus.lib.

Building the repository in either case is recommended as it will also create a fake agent executable to aid testing of your game server without deploying to the remote ONE Platform.

## Requirements / dependencies

### Windows build tools

- MSVC (Microsoft Visual C++ compiler)
- cmake
- git

The following Windows configurations are tested and supported:

- Windows 10 (VS2019)
  - [Visual Studio Build Tools 2019](https://aka.ms/vs/16/release/vs_buildtools.exe) or Visual Studio 2019
    with: MSVC 142 (v14.29) compiler,  Windows 10 SDK (10.0.10941.0)
  - [cmake 3.17.4 or higher](https://cmake.org/download/)

- Windows 10 (VS2022)
  - [Visual Studio Build Tools 2022](https://aka.ms/vs/17/release/vs_buildtools.exe) or Visual Studio 2022
    with: MSVC 143 (v14.41) compiler,  Windows 11 SDK (10.0.22621.0)
  - [cmake 3.23 or higher](https://cmake.org/download/)

### Linux build tools

Linux (Ubuntu and Debian are supported)

- build-essential package (gcc/g++/make)
- cmake
- libssl-dev (openssl)
- git

Dependencies can be installed using apt:

```sh
sudo apt update
sudo apt install build-essential cmake libssl-dev git
```

When compiling for 32bit on a 64bit linux system additional 32bit dependencies are required and can be installed using apt:

```sh
sudo apt install install gcc-multilib g++-multilib libssl-dev:i386
```

The following Linux configurations are tested and supported:

- Ubuntu20.04
  - GCC/G++ (v4.9.3) / make (v4.2.1)
  - cmake 3.16.3 or higher
  - libssl-dev (v1.1.1)

- Ubuntu22.04
  - GCC/G++ (v4.11.2) / make (v4.3.4)
  - cmake 3.22.1 or higher
  - libssl-dev (v3.0.2)

- Debian 12
  - GCC/G++ (v4.12.2) / make (v4.3.4)
  - cmake 3.25.1 or higher
  - libssl-dev (v3.0.13)

## Build steps

After the initial repository clone, make sure to initialize the git submodules. On windows make sure to use git-bash for this:

```git-bash
git submodule update --init --recursive
```

## Documentation Generation

Optional
1. For documentation generation:
    - Windows:
        1. [doxygen](https://www.doxygen.nl/manual/install.html#install_bin_windows)
        2. [graphviz](https://graphviz.org/download/)
    - Ubuntu:
        1. > sudo apt-get update
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

For working on the SDK in Visual Studio Code, IDE tips are located [here](vscode.md).

## Documentation Build

The doxygen documentation is built automatically if CMake is able to find the doxygen installed on your system.

## Testing

As noted above, short tests are run at the end of the cmake build, by default.

Run the following bash scripts from within the tools folder to run longer tests:
- run_tests_debug_long.sh
- run_tests_debug_soak.sh

Use the `build/tests/debug/tests.exe -?` option for all commands.
