# Arcus

## Summary

Arcus is the TCP communication protocol and Message API used to communicate between the ONE Platform Agent and Game Servers. See the [ONE Platform Arcus documentation](https://www.i3d.net/docs/one/odp/Game-Integration/Management-Protocol/Arcus-V2/) for details.

This folder provides an implementation of the Arcus TCP protocol and the two actors that communicate with each other in Arcus - the Server (used by the Game Server) and Client (used by the Agent).

It also provides a C API, containing a C interface for the Server. Game developers integrating the SDK only need the dependencies in this arcus folder and should use the C API for integration.

For easy integration, see the adjacent [Fake Game](../fake/arcus/game/readme.md) folder. It contains a C++ wrapper around the C API that can be used directly in the game as a basis for the final integration.

## Integration Guide

The goal of the integration is for the game server to host an Arcus Server over TCP, allowing the ONE Platform to connect to the server to send and receive Arcus messages.

### Adding the library to the Game

The one/arcus folder must be copied to the project and configured for building.

Alternatively to integrate binaries and headers:

1. Build the repository.
2. Copy the static lib found in build/one/arcus/one_arcus.lib/a.

Either way, the following headers must be included in the game server:

- one/arcus/c_api.h
- one/arcus/c_error.h
- one/arcus/c_platform.h

### Using the API

For C++ game engines, the additional C++ code in one/fake/arcus/game is can be used in your integration to wrap the C API and provide easy-to-use C++ interfaces for the game engine.

The ONE Server Wrapper, defined in one/fake/arcus/game/one_server_wrapper.cpp, is the core of this sample. It is a C++ header/cpp wrapper around the ONE Arcus Server API that:

- isolates the ONE API from the rest of the game code
- contains in-source code comments explaining the motivation and purpose of the API calls from the user's perspective
- can be directly copied and used as a head-start for developers integrating the library into their own engines

See the [Fake Game readme](../one/fake/arcus/game/readme.md) for details.

### Testing and Deploying

While developing the integration, the Game Server may be tested by building and running the [Fake Agent](../one/fake/arcus/agent/readme.md). The fake agent can connect to a running instance of the game server, to test the basics of the integration without needing to deploy to the remote ONE Platform.

Once the integration is complete, the Game Server must be deployed to the ONE Platform for final testing. This consists of the following steps:

1. [Setup](https://www.i3d.net/docs/one/odp/Platform-Overview/) an application on the ONE Platform.

2. [Deploy](https://www.i3d.net/docs/one/odp/Platform-Processes/Deployment-Process/) the Game Server with the Arcus integration to the ONE Platform.

3. Use the platform dashboard and tools to confirm the Agent is successfully connected with the server and that all messages are functioning as expected.

### Considerations

#### Network Activity

The One Arcus API has relatively low network activity. It's rare for messages to be sent and received. The most common messages will be small keep-alive packets sent and received several times a minute.

## Design

The design separates the following major concerns:

1. A cross-platform [Socket](internal/socket.h).
2. [Codec](internal/codec.h) to convert to and from the Arcus TCP Protocol data.
3. Base [Connection](internal/connection.h) to manage an active socket that can send/receive messages using the codec.
4. [Payload and Message](message.h) types representing the Arcus message data.
5. A [Server](server.h) and [Client](client.h) that use all the above internally to represent the Arcus Server and Arcus Client.
