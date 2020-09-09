#pragma once

#include <one/arcus/client.h>
#include <one/arcus/error.h>

#include <functional>

namespace one {

class Message;
class Array;

/// Agent can connect to a Server and simulate production deployment behavior.
class Agent final {
public:
    Agent() = default;
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

    // Set error_response callback
    Error set_error_response_callback(std::function<void(void *)> callback, void *data);

    // Set live_state callback.
    Error set_live_state_response_callback(
        std::function<void(void *, int player, int max_player, const std::string &name,
                           const std::string &map, const std::string &mode,
                           const std::string &version)>,
        void *data);

    // Set error_response callback
    Error set_host_information_request_callback(std::function<void(void *)> callback,
                                                void *data);

    Client &client() {
        return _client;
    }

private:
    Client _client;
};

}  // namespace one
