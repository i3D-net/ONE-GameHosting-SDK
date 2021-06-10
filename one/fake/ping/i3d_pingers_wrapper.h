#pragma once

#include <functional>
#include <mutex>
#include <string>

//----------------------------------------------
// ONE SDK object types forward declarations

struct I3dIpList;
typedef I3dIpList *I3dIpListPtr;

struct I3dPingers;
typedef I3dPingers *I3dPingersPtr;

//--------------------------
// C++ wrapping of ping api.

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

    bool init(I3dIpListPtr ip_list);
    void shutdown();

    // Must called often (e.g. each frame). Updates the Ping Cient, which
    // update the i3D game server ping latency information.
    bool update(bool quiet);

    enum class Status { uninitialized = 0, initialized, unknown };
    static std::string status_to_string(Status status);
    Status status() const;

    struct PingStatistics {
        PingStatistics()
            : last_time(0)
            , average_time(0.0)
            , min_time(0)
            , max_time(0.0)
            , median_time(0)
            , ping_response_count(0) {}

        int last_time;
        double average_time;
        int min_time;
        int max_time;
        double median_time;
        unsigned int ping_response_count;
    };

    bool size(unsigned int &size) const;
    bool statistics(unsigned int pos, PingStatistics &statistics) const;

    bool at_least_one_site_has_been_pinged(bool &result) const;
    bool all_sites_have_been_pinged(bool &result) const;

private:
    // The Pingers itself.
    mutable std::mutex _wrapper;
    I3dPingersPtr _pingers;
};

}  // namespace i3d_ping_integration
