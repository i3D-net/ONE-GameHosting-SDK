// TODO wording
# Fake Game

This folder contains a fake game that uses the ONE Game Hosting SDK server API. The code here can be used both as a reference for how real game servers can integrate and use the Arcus library, or as a way to test the library for correctness.

## Dependencies

A game integration requires the Arcus Server library located in one/server.

## ONE Arcus Server Wrapper

The `game::OneServerWrapper` class, located in one_server_wrapper.h/.cpp, encapsulates the Arcus library's `c_api.h` calls within the game. The parsing.h/cpp files are utilities helping to read and write from key/value data in the Arcus messages.

The integration is intended to be as simple as possible, but please let us know if you have any suggestions for improvement.

Below, the main points of an integration are summarized, however the one_server_wrapper.cpp may be referenced for a complete example.

### 1. Initialize, update and shutdown the ONE Game Hosting Arcus Server

The aforementioned one_server_wrapper.cpp file handles all of the following however they are described here to illustrate the main usage pattern of c_api.h.

Initialization:
```c++
// Set custom allocator, if needed.
// one_allocator_set_alloc(my_alloc);
// one_allocator_set_free(my_free);

OneServerPtr server;
// The first parameter is an optional logger, second is port, third is a pointer
// to the server, which will be set to the newly created server.
OneError err = one_server_create(null, 12345, &server);
if (one_is_error(err)) {
    // handle error...
}

// Optional - set custom logger.
OneError err = one_server_set_logger(server, myLogFn, &myLogger);
if (one_is_error(err)) {
    // handle error...
}

// Set callbacks for incoming messages from the ONE platform:
OneError err one_server_set_soft_stop_callback(server, soft_stop, this)
if (one_is_error(err)) {
    // handle error...
}
// See the "Arcus incoming message handlers" section of c_api.h for more functions.

Update to service the connection and messages:
```c++
OneError err = one_server_update(server);
if (one_is_error(err)) {
    // handle error...
    // Note that certain errors, such as ONE_ERROR_SOCKET_BIND_FAILED or
    // ONE_ERROR_SERVER_RETRYING_LISTEN may be part of "normal" behavior, for
    // example if a server restarts quickly and the port hasn't been recycled
    // by the operating system yet.
}

```

Cleanup:
```c++
// Destroy clears the server memory, which also shuts down any active
// connection.
one_server_destroy(server);
```

### 2. Set state for outgoing messages

```c++
OneError err = one_server_set_live_state(server, players, max_players,
                                   name, map, mode, version, nullptr);
if (one_is_error(err)) {
    // handle error...
}
// See the "Arcus outgoing property setters" section of c_api.h for more functions.
```

### 3. Read incoming messages

The callbacks, if registered as in the above initialization code, will be called during the call to one_server_update when the respective message is received.

Example of soft stop callback in an integration:
```c++
// Tell the server to shutdown at the next appropriate time for its users (e.g.,
// after a match end).
void OneServerWrapper::soft_stop(void *userdata, int timeout_seconds) {
    // This example assumes userdata was passed to the callback registration.
    if (userdata == nullptr) {
        // Handle error...
        return;
    }

    auto wrapper = reinterpret_cast<OneServerWrapper *>(userdata);

    // Trigger the game server process to exit gracefully...
}
```
