#pragma once

namespace one {

class Message;

/// Agent can connect to a Server and simulate production deployment behavior.
class Agent
{
public:
    // Todo: ip + port or combined as "ip:port" str?
    Agent(const char* ip, int port);
    ~Agent();

    // Connect to server.

    // Check status.
    int status();

    // Send soft stop.

    // Request server info.

    // Send custom command.
    void send(Message* message, int* error);
private:
    // Arcus connection.
};

} // namespace one