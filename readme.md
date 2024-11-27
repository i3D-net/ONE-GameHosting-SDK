# i3D.net ONE Game Hosting SDK

The i3D ONE Game Hosting platform is dynamic scaling and deployment service for game servers. This SDK facilitates communication between game servers and the hosting platform using our [Arcus component](one/arcus/readme.md). An Arcus server is hosted on the game server and the Hosting Platform connects to the game server using an Agent.

Additionally the SDK offers a [Ping client library](one/ping/readme.md) that can be run on client game to assist in matchmaking.

Finally the SDK offers accompanying SDK tools and documentation for testing, integration and development.

## Documentation outline

The documentation included with this SDK covers all practical requirements to integrate the platform into a game. The complete documentation for i3D ONE Game Hosting is available on the [i3D.net website](https://www.i3d.net/docs/one/odp/).

Documentation and integration guides for the individual parts of the sdk can found in their respective folders, as listed in the SDK overview below.

Instructions on how to build and test the complete SDK are also provided:

- [build guide windows](build-windows.md) explains how to build and test the repository on windows.
- [build guide linux](build-linux.md) explains how to build and test the repository on linux systems.

## SDK overview

Version: 1.0.0

- [one/arcus/](one/arcus/readme.md) is the server component that enables communication between the game server and the scaling environment of i3D ONE Game Hosting. It is to be integrated into the game server. It contains a C/C++ implementation of the Arcus protocol and messages and hosts a TCP server for communication.
- [one/ping/](one/ping/readme.md) is a client library that provides a way to obtain ping-server addresses from the ONE backend and offers utilities to ping these servers as part of the matchmaking features in a game's player client code.
- [one/fake/](one/fake/readme.md) provides samples for integration, development and testing of SDK components.
- [tests/](tests/readme.md) contains all automated tests for the SDK source code.
- [tools/](tools/readme.md) provides build scripts, test scripts and start scripts.
- [doxygen/](doxygen/readme.md) is the configuration and build target folder for documentation that is automatically generated from the source code during a build.

The i3D.net ONE Game Hosting SDK is supported on both Windows and Linux systems, please file any issues [here](https://github.com/i3D-net/ONE-GameHosting-SDK/issues).

### Major Goals

1. C/C++ v11 library.
2. Easy-to-use C API interface for maximum language compatibility.
3. Source-only build allowing for "copy and drop-in" of files.
4. Supported platforms:
    - Windows 10 Pro with Visual Studio 2017, 2019 or VSCode
    - Ubuntu 18.04, Ubuntu 20.04, Ubuntu 22.04 with VSCode

## Game Engine Plugins

### Unity

The [ONE-GameHosting-SDK-Unity](https://github.com/i3D-net/ONE-GameHosting-SDK-Unity) github repository presents a hosting plugin, a client plugin and example projects for Unity.

### Unreal Engine

The [ONE-GameHosting-SDK-Unreal](https://github.com/i3D-net/ONE-GameHosting-SDK-Unreal) github repository offers a hosting plugin, a client plugin and example projects for Unreal Engine. The plugins can also be installed from the [Unreal Marketplace](https://www.unrealengine.com/marketplace/en-US/profile/i3D.net+Performance+Hosting).
