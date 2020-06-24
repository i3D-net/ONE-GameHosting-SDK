# Fake Agent

In One, the Agent is the component that connects to and communicates to the Game Server to send and receive Arcus API messages. In production, it lives alongside the deployed Game Server process.

The Fake Agent provided here uses the arcus library to simulate all of the agent behavior that is possible on a production One deployment. It is used to connect to and test a Server of the One Game Hosting SDK.

It is *not* intended to be included in the game server's build nor is it required to build the server or accompanying arcus folder libraries.