# Arcus

Arcus is the TCP communication protocol and API used to communicate between the One Platform Agent and Game Servers.

This folder provides an implementation of the Arcus TCP protocol and the two actors that communicate with each other in Arcus - the Server (Game Server) and Client (Agent).

This library is used by the adjacent server folder library that provides the
game server API, and thus must be included into the game servers build system. The arcus library is not considered part of the public API and should not be interfaced with directly as it is subject to arbitrary changes not affecting the main C API.

The design separates the following major concerns:

- cross-platform [Socket](internal/socket.h)
- [Codec](internal/codec.h) to convert to and from the Arcus TCP Protocol
- Base [Connection](internal/connection.h) to manage an active socket that can send/receive messages using the codec
- [Payload and Message](message.h) types representing the Arcus message data
- Main [Arcus](arcus.h) interface wrapping the above to provide a message API for a versioned connection
