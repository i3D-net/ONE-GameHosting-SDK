#pragma once

#include <one/arcus/arcus.h>

namespace one {

class Message;

/// Agent can connect to a Server and simulate production deployment behavior.
class Agent {
public:
    Agent() = default;
    ~Agent() = default;

    // Connect to server.
    int connect(const char* ip, int port);

    // Check status.
    int status();

    int update();

    // Send soft stop.
    int send_soft_stop();

    // Request server info.
    int request_server_info();

    // Send custom command.
    int send(Message* message);

private:
    Client _client;
};

}  // namespace one
