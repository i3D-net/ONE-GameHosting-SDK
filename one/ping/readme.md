# Ping

## Summary

This folder provides Ping features to determine latency to the ONE server platform for use in matchmaking.

This folder contains all dependencies, exposed via the [C API](c_api.h).

For easy integration, see the [fake ping](../fake/ping/readme.md) that uses the ping component. It contains a C++ wrapper around the C API that can be used directly in the game as a basis for the final integration.

## Integration Guide - Ping Component

The goal of the integration is to obtain network ping latency values from the player to the game server locations.

### Adding the library to the Game

The one/ping folder must be copied to the project and configured for building.

Alternatively to integrate binaries and headers:

1. Build the repository.
2. Copy the binaries found in build/one/ping.

Either way, the following headers must be included in the game server:

- one/ping/c_api.h
- one/ping/c_error.h
- one/ping/c_platform.h

### Using the API

For C++ game engines, the additional C++ code in one/fake/ping can be used in your integration to wrap the C API and provide easy-to-use C++ interfaces for the game engine.

The sites and pingers wrappers, located at one/fake/ping/i3d_sites_getter_wrapper.cpp and one/fake/ping/i3d_pingers_wrapper.cpp, are the core of the sample integration. They are C++ header/cpp wrappers around the i3d Ping C API that:

- isolates the ONE API from the rest of the game code
- contains in-source code comments explaining the motivation and purpose of the API calls from the user's perspective
- can be directly copied and used as a head-start for developers integrating the library into their own engines

See the [Fake Game readme](../one/fake/ping/readme.md) for details.

### Testing

No special testing configuration is needed. Integrate the ping component into your game and inspect the results for correctness.

### Considerations

#### Performance

The library performs HTTP calls (via your engine's own solution), decodes JSON, and makes outgoing TCP ping calls. The frequency of this activity can be controlled directly by your integration.

## Design

The design separates the following major concerns:

1. A cross-platform [ICMP Socket](internal/icmp_socket.h).
2. Obtaining the i3D back end server locations. See the `i3d_ping_sites_XXX` functions in the [C API](c_api.h).
3. Pinging a list of addresses. See the `i3d_ping_pingers_XXX` functions in the [C API](c_api.h).
4. Handling a list of addresses. See the `i3d_ping_ip_list_XXX` functions in the [C API](c_api.h).
