# i3D.net ONE Game Hosting SDK

**Version: v0.9.1 (Beta)**
> All v1.0 features are complete and ready for integration and use. Customer iteration will determine any final changes before labelling as v1.0.

---

The SDK provides games with the ability to communicate with the i3D.net ONE Platform, for easy scaling of game servers!

- [Build](docs/build.md) – How to build and test the repository.
- [Integration Guide](docs/integration_guide.md) – How to integrate the Arcus API into a Game Server.

The i3D.net Game Hosting SDK works on Windows and Linux.
If something doesn’t work, please [file an issue](https://github.com/i3D-net/ONE-GameHosting-SDK/issues).

## Overview

The documentation for the entire ONE Platform can be found [here](https://www.i3d.net/docs/one/), however the previously mentioned [Integration Guide](docs/integration_guide.md) is all that is needed to integrate Arcus into the Game Server itself.

### Major Goals

1. C/C++ v11 library.
2. Easy-to-use C API interface for maximum language compatibility.
3. Source-only build allowing for "copy and drop-in" of files.
4. Supported platforms:
    - Windows 10 Pro, Visual Studio 2017 and VSCode
    - Ubuntu 18.04, VSCode

### Not Included

The below are not provided or part of current goals. Please [add an issue](https://github.com/i3D-net/ONE-GameHosting-SDK/issues) for change requests!

1. Testing on Linux distros other than Ubuntu 18.04.

### Layout

The SDK is made up of code to be integrated into the Game plus additional components that aid in the testing, development and integration.

1. [Arcus](one/arcus/readme.md) is the library that provides communication between the Game Server and the scaling environment in the One Platform. It is to be integrated into the Game Server. It contains a C/C++ implementation of the ONE platform's Arcus protocol and messages, and provides a TCP server to communicate with the ONE Platform.
2. [Ping](one/ping/readme.md) is a library that provides a way to obtain server addresses of the ONE backend and utilities to ping the servers, for use in a game's player client code's matchmaking features.
3. [Fake](one/fake/readme.md) contains samples that use the SDK components to aid in integration, development and testing.
4. [Tests](tests/readme.md) contains all automated tests for the SDK source.
5. [Docs](docs/readme.md) contains additional readme files and DOxygen configuration.

## Engine Plugins

### Unity

Unity engine support is implemented as a plug-in which is available [here](https://github.com/i3D-net/ONE-GameHosting-SDK-Unity).

### Unreal

Unreal Engine support is implemented as a plug-in which is available [on the asset store](https://www.unrealengine.com/marketplace/en-US/product/i3d-net-game-hosting-platform-sdk) and also on [github](https://github.com/i3D-net/ONE-GameHosting-SDK-Unreal).