#pragma once

#include <one/fake/ping/i3d_pingers_wrapper.h>
#include <one/fake/ping/i3d_sites_getter_wrapper.h>

#include <chrono>
#include <functional>
#include <string>
#include <mutex>

using namespace std::chrono;

namespace i3d_ping_integration {

/// A fake i3D Ping Client. It owns a I3dPingClientWrapper member that encapsulates
/// the c_api.h of the ping client.
///
/// The Fake Ping's purpose is to:
/// - Facilitate automated and manual integration testing of the SDK
/// - Illustrate an integration
///
/// Please note that this particular class is a completely fake ping, and its
/// interface is not meant to represent those found in real ping clients. Its public
/// surface's main goal is to assist integration-style testing. See the use of
/// its _i3d_ping_client member variable for integration functionality.
class Ping final {
public:
    Ping();
    Ping(const Ping &) = delete;
    Ping &operator=(const Ping &) = delete;
    ~Ping();

    //------------
    // Life cycle.

    bool init();
    void shutdown();

    void update();

    // Exposed for testing purposes, however use of this should be kept to a
    // minimum or removed. The game, as much as reasonable, should be tested as
    // a black box.
    // I3dPingClientWrapper &I3dPingClientWrapper() {
    //     return _i3d_ping_client;
    // }

    // Exposed for testing purposes to avoid spamming std::error when testing for expected
    // failures.
    void set_quiet(bool quiet) {
        _quiet = quiet;
    }

    // Exposed for testing purposes to avoid spamming std::error when testing for expected
    // failures.
    bool is_quiet() const {
        return _quiet;
    }

private:
    // I3dPingClientWrapper _i3d_ping_client;

    mutable std::mutex _ping;

    // Log level.
    bool _quiet;
};

namespace allocation {
// Return number of calls to alloc made on the custom allocator.
size_t alloc_count();

// Returns number of calls to free made on the custom allocator.
size_t free_count();
}  // namespace allocation

}  // namespace i3d_ping_integration
