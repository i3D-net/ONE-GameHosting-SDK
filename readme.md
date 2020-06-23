# i3D Game Hosting SDK

Version: Technical Preview

Providing game servers the ability to communicate with the i3D One Platform.

## v1.0 Plan

### Goals

The first release, v1.0, will focus on the following:

1. C/C++ v11 library.
2. Windows 10 Pro, tested w/ Visual Studio 2017 and VSCode.
3. Ubuntu 18.04 tested with VSCode.
4. Pure C API interface.
5. Source-only build - "copy & drop-in" of files for building

### Not Included

The below are not currently part of v1.0 goals, however they are on our radar. Please notify us of change requests!

1. Building of static and dll binaries.
2. Specific Unity or Unreal engine support beyond usage of C API to create own custom integration.
3. Build testing on CentOS or other Linux distros.

## Overview

The SDK is made up of code to be integrated into the Game Server and other components to aid in testing, development and integration of the Server components.

1. [Server](one/server/readme.md) is the main interface integrated into a Game Server to communicate with One. See the C API [source file](one/server/c_api.h) for the main interfaces that the game server integration must use.
2. [Arcus](one/arcus/readme.md) is used by the [Server]() but also by the test and integration components.
3. [Game](one/game/readme.md) is a fake Game used in testing, development and as a guide and reference for integration of the [Server]() into a Game Server.
4. [Agent](one/agent/readme.md) is a fake One Agent Client used for testing and development. It simulates the behavior of a One Agent connecting to a real Deployment on the One platform.

The [tests](one/tests/readme.md) and [docs](one/docs/readme.md) serve their respective purposes.

## Source Documentation and Integration Guide

Reference the prebuilt docs on the i3d website, or follow the build instructions to generate the documentation from the repository.

## Build Requirements

1. Doxygen.
    - [Windows](https://www.doxygen.nl/manual/install.html#install_bin_windows)
    - Ubuntu:
        1. `sudo apt-get update`
        2. `sudo apt-get install doxygen`
2. CMake. See CMakeLists.txt in the root for tested versions.
3. C++ toolchain. Currently tested environments:
    - Windows 10 Pro w/ VS2017
    - Ubuntu 18.04

## Build

### Configure the build output and initialize cmake

```bash
cmake -s . -B ./build
```

### Build and run tests

#### Windows

```bash
cmake --build ./build --config Release --target ALL_BUILD
```

#### Linux

```bash
cmake --build ./build --config Release --target all
```

The doxygen documentation is built automatically if cmake is able to find the doxygen installed on your system.

By default, the tests will run as part of the build process. Set `RUN_TEST_AFTER_BUILD` to false to disable this behavior.

## IDE Support

### VSCode

#### Extensions

Install the plugins:
    1. C/C++ from Microsoft
    2. C++ Intellisense from austin
    3. CMake from twxs
    4. CMake Tools from Microsoft
    5. Clang-format from xaver

#### Settings example

The default `.vscode/settings.json` config is below will help to bootstrap quickly.

```json
{
    "cmake.buildDirectory": "${workspaceFolder}/build",
    "cmake.sourceDirectory": "${workspaceFolder}",
    "editor.formatOnSave": true,
    "C_Cpp.default.configurationProvider": "ms-vscode.cmake-tools",
}
```

#### CMake integration

In order to use CMake directly with `vscode`, see the [cmake tools](https://vector-of-bool.github.io/docs/vscode-cmake-tools/getting_started.html#cmake-tools-quick-start) online documentation.
