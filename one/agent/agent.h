#pragma once

#include <one/arcus/arcus.h>

#include <functional>

namespace one {

class Message;

/// Agent can connect to a Server and simulate production deployment behavior.
class Agent {
public:
    Agent() = default;
    ~Agent() = default;

    // Connect to server.
    int connect(const char *ip, int port);

    // Check status.
    int status();

    // Update: process incomming message & outgoing messages.
    int update();

    // Send soft stop.
    int send_soft_stop();

    // Request live_state_request message.
    int send_live_state_request();

    // Set live_state callback.
    int set_live_state_callback(
        std::function<void(int player, int max_player, const std::string &name,
                           const std::string &map, const std::string &mode,
                           const std::string &version)>);

private:
    Client _client;
};

}  // namespace one
