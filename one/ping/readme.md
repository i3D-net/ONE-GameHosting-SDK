# Ping

## Summary

This folder provides Ping features to determine latency to the ONE server platform for use in matchmaking.

This folder contains all dependencies, exposed via the [C API](c_api.h).

For easy integration, see the [fake Game Client](../fake/arcus/game/readme.md) that uses the ping component. It contains a C++ wrapper around the C API that can be used directly in the game as a basis for the final integration.

## Design

The design separates the following major concerns:

1. Obtaining the i3D back end server locations. See the `i3d_ping_sites_XXX` functions in the [C API](c_api.h).
2. Pinging a list of addresses. See the `i3d_ping_pingers_XXX` functions in the [C API](c_api.h).
