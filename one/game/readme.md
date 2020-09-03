# Game

A fake game that integrates and uses the One Game Hosting server. The code here can be used both as a reference for how real game servers can integrate and use the server library, or as a way to test the library for correctness.

It is *not* intended to be included in the game server's build nor is it required to build the server or accompanying arcus folder libraries.

# One Server Wrapper

The `game::OneServerWrapper` class is wrapping up the `one` `c_api.h` calls so that it could be used in a game. Mainly it shows how to:

1. init/shutdown the `One Game Hosting` server.
2. register `callbacks` to react to incoming messages.
3. send outgoing message to the `One Game Hosting Agent`.
4. check for errors & get their correcponding description.
