# Integration Guide

The goal of the integration is for the game server to host an Arcus Server over TCP, allowing the One Platform to connect to the server to send and receive Arcus messages.

## Overview

The SDK is made up of code to be integrated into the Game Server and other components to aid in testing, development and integration of the Server components.

1. [Arcus](../one/arcus/readme.md) provides a C/C++ implementation of the One platform's Arcus protocol and messaging API. Game Servers must integrate this component, using the [C API](../one/server/c_api.h).
2. [Game](../one/game/readme.md) is a fake Game used in testing, development and as a guide and reference for integration of the [Arcus C API](../one/arcus/readme.md) into a Game Server.
3. [Agent](../one/agent/readme.md) is a fake One Agent Client used for testing and development. It simulates the behavior of a One Agent connecting to a real Deployment on the One platform.

The [tests](../tests/readme.md) and [docs](../docs/readme.md) folders serve their respective purposes.

## Adding the library to the Game

The one/arcus folder must be copied to the project and configured for building. See the [Fake Game CMakeLists.txt](../one/game/CMakeLists.txt) for a cmake example and reference.

## Using the Arcus Server API

To start, an integration must create an Arcus Server that corresponds with the Game Server:

```c++
    OneError err = one_server_create(&_server);
```
Afterwards, the server must:
- have its message callbacks configured
- be instructed to listen for Arcus connections
- be updated each frame
- be shutdown when the game server host goes away

The [Fake Game](../one/game/readme.md) provides a full sample integration using all API features.

The [One Server Wrapper](../one/game/one_server_wrapper.h) is the core of this sample. It is a C++ header/cpp wrapper around the One API that

- isolates the One API from the rest of the game code
- contains ample in-source code comments explaining the motivation and purpose of the API calls from the user's perspective
- can be directly copied and used as a head-start for developers integrating the library into their own engines

For details, please refer to the [Fake Game](../one/game/readme.md), specifically the [One Server Wrapper](../one/game/one_server_wrapper.h).

## Testing and Deploying

While developing the integration, the Game Server may be tested by building and running the [Fake Agent](../one/agent/readme.md).

Once the integration is complete, the Game Server must be deployed to the One Platform for final testing. This consists of the following steps:
1. [Setup](https://www.i3d.net/docs/one/odp/Platform-Overview/) an application on the One Platform.
2. [Deploy](https://www.i3d.net/docs/one/odp/Platform-Processes/Deployment-Process/) the Game Server with the Arcus integration to the One Platform.
3. Use the platform dashboard and tools to confirm the Agent is successfully connected with the server and that all messages are functioning as expected.

## Considerations

### Network Activity

The One Arcus API has relatively low network activity. It's rare for messages to be sent and received. The most common messages will be small keep-alive packets sent and received several times a minute.