# Server Library

The Server is the component that must be integrated into the game.

It provides an API surface to listen for an incoming connection from the One platform, servicing of the connection, and supplies methods to listen to and send One messages. The `one_game_hosting_api` C function found in c_api.h is the main access point for the API.

It interally uses and depends on the adjacent arcus library folder.

This folder, along with the adjacent arcus library folder, must be integrated into the game server build.