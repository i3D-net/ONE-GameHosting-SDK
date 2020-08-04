#include <one/agent/agent.h>

namespace one {

int Agent::connect(const char* addr, int port) {
    const int error = _client.connect(addr, port);

    if (error != 0) {
        return error;
    }

    return 0;
}

int Agent::status() {
    return 0;
}

int Agent::update() {
    const int error = _client.update();

    if (error != 0) {
        return error;
    }

    return 0;
}

int Agent::send_soft_stop() {
    const int error = _client.send_soft_stop();

    if (error != 0) {
        return error;
    }

    return 0;
}

int Agent::request_server_info() {
    const int error = _client.request_server_info();

    if (error != 0) {
        return error;
    }

    return 0;
}

int Agent::send(Message* message) {
    const int error = _client.send(message);

    if (error != 0) {
        return error;
    }

    return 0;
}

}  // namespace one
