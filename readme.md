# i3D.net Game Hosting SDK

**PREVIEW**
> Work-in-progress functional preview. Not ready for integration. Some Todo's remain. Documentation and source is not yet representative of integration-ready v1.0 release.

---

Providing game servers with the ability to communicate over TCP with the i3D.net One Platform.

- [Build](#Build) – How to build and test the repository.
- [Integration Guide](docs/integration_guide.md) – How to integrate the Arcus API into a Game Server.
- [VSCode Setup](docs/vscode.md) - How to configure VSCode for building the SDK.

The i3D.net Game Hosting SDK works on Windows and Linux.
If something doesn’t work, please [file an issue](https://github.com/i3D-net/ONE-GameHosting-SDK/issues).

## Quick Overview

The documentation for the entire One Platform can be found [here](https://www.i3d.net/docs/one/), however the previously mentioned [Integration Guide](docs/integration_guide.md) is all that is needed to integrate Arcus into the Game Server itself.

### Major Goals

1. C/C++ v11 library.
2. Easy-to-use C API interface for maximum language compatibility.
3. Source-only build - "copy & drop-in" of files for building.
4. Supported platforms:
    - Windows 10 Pro, Visual Studio 2017 and VSCode
    - Ubuntu 18.04, VSCode

### Not Included

The below are not provided or part of current goals. Please [add an issue](https://github.com/i3D-net/ONE-GameHosting-SDK/issues) for change requests!

1. Explicit preparation of headers + static (.lib, .o) and shared (.dll, .so) binaries for the Arcus library. Static libraries are currently output by the build tools.
2. Unity or Unreal engine support beyond usage of the [C API](one/arcus/c_api.h) to create in a custom integration.
3. Testing on CentOS or other Linux distros other than Ubuntu 18.04.

### Layout

The SDK is made up of code to be integrated into the Game Server plus additional components that aid in the testing, development and integration.

1. [Arcus](one/arcus/readme.md) is the library that must be integrated into the Game Server. It is a C/C++ implementation of the One platform's Arcus protocol and messages, and provides a TCP server to communicate with the One Platform.
2. [Game](one/game/readme.md) is a fake Game used in testing, development and as a guide and reference for integration of the [Arcus](one/arcus/readme.md) [C API](one/arcus/c_api.h) into a Game Server.
3. [Agent](one/agent/readme.md) is a fake One Agent Client used for testing and development. It simulates the behavior of a One Agent that connects to and handles all communication between the Game Server and the One Platform.

The [tests](one/tests/readme.md) and [docs](one/docs/readme.md) folders serve their respective purposes.

## Build

### Requirements

1. C++ Toolchain:
    - Windows: [Visual Studio 2017, or build tools (without Editor)](https://visualstudio.microsoft.com/vs/older-downloads/)
    - Linux: GCC (7.5.0 tested)
2. [CMake](https://cmake.org/download/).
3. Optional: For documentation generation:
    - Windows:
        1. [doxygen](https://www.doxygen.nl/manual/install.html#install_bin_windows)
        2. [graphviz](https://graphviz.org/download/)
    - Ubuntu:
        1. `sudo apt-get update`
        2. `sudo apt-get install doxygen graphviz`

### CMake

By default, the tests will run as part of the build process. Set `RUN_TEST_AFTER_BUILD` to `false` to disable this behavior.

CMake is configured to ignore long-running tests in the build
and test process. These tests begin with the "long:" prefix.

The doxygen documentation is built automatically if CMake is able to find the doxygen installed on your system.

**Configure**

 ```bash
 mkdir build
 cmake -S . -B ./build
 ```

The `-S` and `-B` options require CMake 3.13 or newer. Older version must cd into the build folder and run `cmake ..`.

**Build: Windows**

Run the following in the root folder:
```bash
cmake --build ./build --config Release --target ALL_BUILD
```

**Build: Linux**

Run the following in the root folder:
```bash
cmake --build ./build --config Release --target all
```

## Clean build

Delete the ./build folder and run the above build commands.
