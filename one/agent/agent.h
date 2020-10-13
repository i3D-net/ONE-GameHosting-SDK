#pragma once

#include <one/arcus/client.h>
#include <one/arcus/error.h>

#include <functional>
#include <mutex>

namespace i3d {
namespace one {

class Array;
class Object;
class Message;

/// Agent can connect to a Server and simulate production deployment behavior.
class Agent final {
public:
    Agent();
    ~Agent() = default;

    // Init with a target remote address. The agent attempts to connect during
    // update.
    Error init(const char *ip, unsigned int port);

    void set_quiet(bool quiet) {
        _quiet = quiet;
    }

    bool is_quiet() const {
        return _quiet;
    }

    // Update: process incomming message and outgoing messages.
    Error update();

    Error send_soft_stop(int timeout);
    Error send_allocated(Array &array);
    Error send_metadata(Array &array);

    // Set live_state callback.
    Error set_live_state_callback(
        std::function<void(void *, int players, int max_players, const std::string &name,
                           const std::string &map, const std::string &mode,
                           const std::string &version)>,
        void *data);

    // Set application instance set status callback
    Error set_application_instance_status_callback(
        std::function<void(void *, int)> callback, void *data);

    // Exposed for testing purposes, however use of this should be kept to a
    // minimum or removed. The agent, as much as reasonable, should be tested as
    // a black box.
    Client &client() {
        return _client;
    }

    int host_information_send_count() const {
        const std::lock_guard<std::mutex> lock(_agent);
        return _host_information_send_count;
    }

    int application_instance_information_send_count() const {
        const std::lock_guard<std::mutex> lock(_agent);
        return _application_instance_information_send_count;
    }

    int live_state_receive_count() const {
        const std::lock_guard<std::mutex> lock(_agent);
        return _live_state_receive_count;
    }

    int application_instance_status_receive_count() const {
        const std::lock_guard<std::mutex> lock(_agent);
        return _application_instance_status_receive_count;
    }

private:
    Error send_host_information();
    Error send_application_instance_information();

    Client _client;

    bool _quiet;

    int _host_information_send_count;
    int _application_instance_information_send_count;
    int _live_state_receive_count;
    int _application_instance_status_receive_count;

    mutable std::mutex _agent;
};

}  // namespace one
}  // namespace i3d
