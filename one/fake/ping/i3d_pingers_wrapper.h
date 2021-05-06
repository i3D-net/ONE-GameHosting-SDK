#pragma once

#include <functional>
#include <mutex>
#include <string>

//----------------------------------------------
// ONE SDK object types forward declarations

struct I3dPingers;
typedef I3dPingers *I3dPingersPtr;

namespace i3d_ping_integration {

/// I3dPingClientWrapper encapsulates the integration for the i3D Ping Client and
/// provides a ping interface that hides the i3D Ping Client API implementation
/// from the game. Errors are handled directly in the implementation of the
/// wrapper.
///
/// This is provided as a potential copy-paste initial integration solution for
/// users that would like to hide the c-api under a game-specific C++ class, and
/// also as a complete example of an integration.
class I3dPingersWrapper final {
public:
    I3dPingersWrapper();
    I3dPingersWrapper(const I3dPingersWrapper &) = delete;
    I3dPingersWrapper &operator=(const I3dPingersWrapper &) = delete;
    ~I3dPingersWrapper();

    //------------
    // Life cycle.

    struct AllocationHooks {
        // The hooks are optional, they can be left null.
        AllocationHooks() : alloc(nullptr), free(nullptr), realloc(nullptr) {}
        AllocationHooks(std::function<void *(size_t)> alloc_in,
                        std::function<void(void *)> free_in,
                        std::function<void *(void *, size_t)> realloc_in)
            : alloc(alloc_in), free(free_in), realloc(realloc_in) {}

        std::function<void *(size_t)> alloc;
        std::function<void(void *)> free;
        std::function<void *(void *, size_t)> realloc;
    };

    // alloc and free are optional allocation override handlers. Both may be nullptr,
    // otherwise both are required.
    bool init(const AllocationHooks &hooks);
    void shutdown();

    // Must called often (e.g. each frame). Updates the Ping Cient, which
    // update the i3D game server ping latency information.
    void update(bool quiet);

    enum class Status { uninitialized = 0, initialized, unknown };
    static std::string status_to_string(Status status);
    Status status() const;

private:
    // The Arcus Server itself.
    mutable std::mutex _wrapper;
    I3dPingersPtr _pingers;
};

}  // namespace i3d_ping_integration
