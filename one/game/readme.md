# Game

A fake game that integrates and uses the One Game Hosting server. The code here can be used both as a reference for how real game servers can integrate and use the Arcus library, or as a way to test the library for correctness.

The fake game source is *not* intended to be included in the game server's build nor is it required to the arcus folder library.

# One Server Wrapper

The `game::OneServerWrapper` class encapsulates the Arcus library's `c_api.h` calls so that it could be used in a game. Mainly it shows how to:

1. Initialize, update and shutdown the One Game Hosting Arcus Server.
2. Register callbacks to react to incoming messages.
3. Send outgoing messages to the connected client, a One Game Hosting Agent.
4. Handle errors.
