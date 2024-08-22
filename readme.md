# i3D.net [ONE Game Hosting](https://www.i3d.net/docs/one/odp/) SDK

The ONE Game Hosting SDK facilitates communication with the i3D.net ONE Platform, providing seamless scaling for game servers!

- [Build](docs/build.md) – How to build and test the repository.
- [Integration Guide](docs/integration_guide.md) – How to integrate the Arcus API into a Game Server.

The i3D.net Game Hosting SDK works on Windows and Linux.
If something doesn’t work, please [file an issue](https://github.com/i3D-net/ONE-GameHosting-SDK/issues).

## Overview

Version: 1.0.0

The complete documentation for the ONE Platform can be found [here](https://www.i3d.net/docs/one/), however the previously mentioned [Integration Guide](docs/integration_guide.md) covers all requirements to integrate the ONE Platform into a Game Server.

### Major Goals

1. C/C++ v11 library.
2. Easy-to-use C API interface for maximum language compatibility.
3. Source-only build allowing for "copy and drop-in" of files.
4. Supported platforms:
    - Windows 10 Pro with Visual Studio 2017, 2019, 2022 and VSCode
    - Ubuntu 18.04, Ubuntu 20.04, Ubuntu 22.04 with VSCode

### Layout

The SDK consists of code to support game integration of the One Game Hosting service and provides addional components that aid in testing and development.

1. [Arcus](one/arcus/readme.md) is the server library that enables communication between the Game Server and the scaling environment in the One Hosting Platform. It is to be integrated into the Game Server. It provides a TCP server for communication and contains a C/C++ implementation of the Arcus protocol and messages.
2. [Ping](one/ping/readme.md) is a client library that provides a way to obtain ping-server addresses from the ONE backend and offers utilities to ping these servers as part of the matchmaking features in a game's player client code.
3. [Fake](one/fake/readme.md) contains samples for integration, development and testing of SDK components. 
4. [Tests](tests/readme.md) contains all automated tests for the SDK source code.
5. [Docs](docs/readme.md) contains additional readme files and DOxygen configuration.

## Game Engine Plugins

### Unity

The [ONE-GameHosting-SDK-Unity github repository](https://github.com/i3D-net/ONE-GameHosting-SDK-Unity) offers Unity support via a hostingplugin, a clientplugin and example projects.

### Unreal Engine

The [ONE-GameHosting-SDK-Unreal github repository](https://github.com/i3D-net/ONE-GameHosting-SDK-Unreal) offers Unity support via a hosting plugin, a client plugin and example projects. The plugins can also be installed via the [Unreal marketplace](https://www.unrealengine.com/marketplace/en-US/profile/i3D.net+Performance+Hosting).
