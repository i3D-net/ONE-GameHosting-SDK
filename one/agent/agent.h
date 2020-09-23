#pragma once

#include <one/arcus/client.h>
#include <one/arcus/error.h>

#include <functional>

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

    // Update: process incomming message & outgoing messages.
    Error update();

    // Send soft stop.
    Error send_soft_stop_request(int timeout);

    // Request live_state_request message.
    Error send_live_state_request();

    // Request allocated_request message.
    Error send_allocated_request(Array *array);

    // Request allocated_request message.
    Error send_meta_data_request(Array *array);

    // Set live_state callback.
    Error set_live_state_response_callback(
        std::function<void(void *, int players, int max_players, const std::string &name,
                           const std::string &map, const std::string &mode,
                           const std::string &version)>,
        void *data);

    // Set error_response callback
    Error set_host_information_request_callback(std::function<void(void *)> callback,
                                                void *data);

    // Exposed for testing purposes, however use of this should be kept to a
    // minimum or removed. The agent, as much as reasonable, should be tested as
    // a black box.
    Client &client() {
        return _client;
    }

    int player_join_call_count() const {
        return _player_join_call_count;
    }

    int player_left_call_count() const {
        return _player_left_call_count;
    }

private:
    Client _client;

    int _player_join_call_count;
    int _player_left_call_count;
};

}  // namespace one
}  // namespace i3d
