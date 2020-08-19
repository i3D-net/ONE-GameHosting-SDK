# Arcus

Arcus is the TCP communication protocol and API used to communicate between the One Platform Agent and Game Servers.

This folder provides an implementation of the Arcus TCP protocol and the two actors that communicate with each other in Arcus - the Server (Game Server) and Client (Agent).

It also provides a C API, containing a C interface for the Server. Game developers integrating the SDK only need the dependencies in this arcus folder and use the C API for integration.

For an example and reference on how to use the Arcus Server C API, see the game.cpp file in the adjacent game folder.

** NOTE **
The C API is the public interface that should be used by external game integrations to ensure minimal interface changes.

The design separates the following major concerns:

- A cross-platform [Socket](internal/socket.h)
- [Codec](internal/codec.h) to convert to and from the Arcus TCP Protocol
- Base [Connection](internal/connection.h) to manage an active socket that can send/receive messages using the codec
- [Payload and Message](message.h) types representing the Arcus message data
- A [Server](server.h) and [Client](client.h) that use all the above internally to represent the Arcus Server and Arcus Client.

