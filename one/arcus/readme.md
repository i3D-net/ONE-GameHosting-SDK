# Arcus

## Summary

Arcus is the TCP communication protocol and Message API used to communicate between the ONE Platform Agent and Game Servers. See the [ONE Platform Arcus documentation](https://www.i3d.net/docs/one/odp/Game-Integration/Management-Protocol/Arcus-V2/) for details.

This folder provides an implementation of the Arcus TCP protocol and the two actors that communicate with each other in Arcus - the Server (used by the Game Server) and Client (used by the Agent).

It also provides a C API, containing a C interface for the Server. Game developers integrating the SDK only need the dependencies in this arcus folder and should use the C API for integration.

For easy integration, see the adjacent [Fake Game](../game/readme.md) folder. It contains a C++ wrapper around the C API that can be used directly in the game as a basis for the final integration.

## Design

The design separates the following major concerns:

1. A cross-platform [Socket](internal/socket.h).
2. [Codec](internal/codec.h) to convert to and from the Arcus TCP Protocol data.
3. Base [Connection](internal/connection.h) to manage an active socket that can send/receive messages using the codec.
4. [Payload and Message](message.h) types representing the Arcus message data.
5. A [Server](server.h) and [Client](client.h) that use all the above internally to represent the Arcus Server and Arcus Client.
