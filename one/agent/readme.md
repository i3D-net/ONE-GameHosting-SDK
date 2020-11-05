# Fake Agent

In One, the Agent is the component that connects to and communicates to the Game Server to send and receive Arcus API messages. In production, it lives alongside the deployed Game Server process.

The Fake Agent provided here uses the arcus library to simulate all of the agent behavior that is possible on a production One deployment. It is used to connect to and test a Server of the One Game Hosting SDK.

It is *not* intended to be included in the game server's build nor is it required to build the server or accompanying arcus folder libraries.

### Connecting against your game server with the fake agent.

1. Build the repository.
2. Run run_fake_agent_release.sh (or debug, as needed).
3. The agent will run, and connect to the game server immediately.

### Using the fake game.

The fake game can be run instead of your game server to ensure things are working as expected in your environment.

1. Build the repository.
2. Run run_fake_game_release.sh (or debug, as needed).
3. The server will listen, and accept an incoming agent connection.