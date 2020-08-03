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

## Server Library

The Server is the component that must be integrated into the game.

It provides an API surface to listen for an incoming connection from the One platform, servicing of the connection, and supplies methods to listen to and send One messages. The `one_game_hosting_api` C function found in c_api.h is the main access point for the API.

It interally uses and depends on the adjacent arcus library folder.

This folder, along with the adjacent arcus library folder, must be integrated into the game server build.
