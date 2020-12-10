# i3D.net ONE Game Hosting SDK

**Version: v0.9 (Beta)**
> All v1.0 features are complete and ready for integration and use. Customer iteration will determine any final changes before labelling as v1.0.

---

The SDK provides game servers with the ability to communicate over TCP with the i3D.net ONE Platform, for easy and efficient scaling of game servers!

- [Build](docs/build.md) – How to build and test the repository.
- [Integration Guide](docs/integration_guide.md) – How to integrate the Arcus API into a Game Server.

The i3D.net Game Hosting SDK works on Windows and Linux.
If something doesn’t work, please [file an issue](https://github.com/i3D-net/ONE-GameHosting-SDK/issues).

## Overview

The documentation for the entire ONE Platform can be found [here](https://www.i3d.net/docs/one/), however the previously mentioned [Integration Guide](docs/integration_guide.md) is all that is needed to integrate Arcus into the Game Server itself.

### Major Goals

1. C/C++ v11 library.
2. Easy-to-use C API interface for maximum language compatibility.
3. Source-only build - "copy and drop-in" of files for building.
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

1. [Arcus](one/arcus/readme.md) is the library that must be integrated into the Game Server. It is a C/C++ implementation of the ONE platform's Arcus protocol and messages, and provides a TCP server to communicate with the ONE Platform.
2. [Game](one/game/readme.md) is a fake Game used in testing, development and as a guide and reference for integration of the [Arcus C API](one/arcus/c_api.h) into a Game Server.
3. [Agent](one/agent/readme.md) is a fake ONE Agent Client used for testing and development. It simulates the behavior of a ONE Agent that connects to and handles all communication between the Game Server and the ONE Platform.

The [tests](tests/readme.md) and [docs](docs/readme.md) folders serve their respective purposes.
