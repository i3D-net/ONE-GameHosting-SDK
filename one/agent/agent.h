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

    // Update: process incomming message & outgoing messages.
    Error update();

    // Send soft stop.
    Error send_soft_stop(int timeout);

    // Request live_state_request message.
    Error send_live_state_request();

    // Request allocated message.
    Error send_allocated(Array *array);

    // Request allocated message.
    Error send_metadata(Array *array);

    // Set live_state callback.
    Error set_live_state_response_callback(
        std::function<void(void *, int players, int max_players, const std::string &name,
                           const std::string &map, const std::string &mode,
                           const std::string &version)>,
        void *data);

    // Set player joined event callback
    Error set_player_joined_event_callback(std::function<void(void *, int)> callback,
                                           void *data);

    // Set player left callback
    Error set_player_left_callback(std::function<void(void *, int)> callback, void *data);

    // Set host information callback
    Error set_host_information_request_callback(std::function<void(void *)> callback,
                                                void *data);

    // Set application instance information callback
    Error set_application_instance_information_request_callback(
        std::function<void(void *)> callback, void *data);

    // Set application instance get status callback
    Error set_application_instance_get_status_request_callback(
        std::function<void(void *)> callback, void *data);

    // Set application instance set status callback
    Error set_application_instance_set_status_request_callback(
        std::function<void(void *, int)> callback, void *data);

    // Exposed for testing purposes, however use of this should be kept to a
    // minimum or removed. The agent, as much as reasonable, should be tested as
    // a black box.
    Client &client() {
        return _client;
    }

    int live_state_call_count() const {
        const std::lock_guard<std::mutex> lock(_agent);
        return _live_state_call_count;
    }

    int player_join_call_count() const {
        const std::lock_guard<std::mutex> lock(_agent);
        return _player_join_call_count;
    }

    int player_left_call_count() const {
        const std::lock_guard<std::mutex> lock(_agent);
        return _player_left_call_count;
    }

    int host_information_call_count() const {
        const std::lock_guard<std::mutex> lock(_agent);
        return _host_information_call_count;
    }

    int application_instance_information_call_count() const {
        const std::lock_guard<std::mutex> lock(_agent);
        return _application_instance_information_call_count;
    }

    int application_instance_get_status_call_count() const {
        const std::lock_guard<std::mutex> lock(_agent);
        return _application_instance_get_status_call_count;
    }

    int application_instance_set_status_call_count() const {
        const std::lock_guard<std::mutex> lock(_agent);
        return _application_instance_set_status_call_count;
    }

private:
    Client _client;

    bool _quiet;

    int _live_state_call_count;
    int _player_join_call_count;
    int _player_left_call_count;
    int _host_information_call_count;
    int _application_instance_information_call_count;
    int _application_instance_get_status_call_count;
    int _application_instance_set_status_call_count;

    mutable std::mutex _agent;
};

}  // namespace one
}  // namespace i3d
