# Arcus

Arcus is the TCP communication protocol and API used to communicate between the One Platform Agent and Game Servers.

This folder provides an interface for the internal implementation of the Arcus TCP protocol and One Agent API management behavior.

This library is used by the adjacent server folder library, and thus must be included into the game servers build system. The arcus library is not considered part of the public API and should not be interfaced with directly as it is subject to arbitrary changes across One Game Hosting SDK versions.

The design separates major concerns.

**Socket**

internal/socket.h/cpp

**Codec**

internal/codec.h/cpp

**Listener**

internal/connection.h/cpp

**Connection**

internal/connection.h/cpp

**API**

arcus.h/cpp

Finally, the higher level Arcus API behavior uses the above internal features to validate expected Arcus Message types, and provides interfaces to deal with translated messages.