#include <one/agent/agent.h>
#include <one/arcus/array.h>
#include <one/game/log.h>

#include <chrono>
#include <thread>

using namespace std::chrono;
using namespace i3d::one;

void sleep(int ms) {
    std::this_thread::sleep_for(milliseconds(ms));
}

int main(int argc, char **argv) {
    const int default_port = 19001;
    int port = default_port;

    if (argc == 2) {
        port = strtol(argv[1], nullptr, 10);

        if (port <= 0) {
            L_ERROR("invalid port provided");
            return 1;
        }
    }

    const std::string address = "127.0.0.1";

    Agent agent;
    auto err = agent.init(address.c_str(), port);
    if (is_error(err)) {
        L_ERROR("failed to init agent.");
        return 1;
    }

    L_INFO("agent is initialized.");
    L_INFO("-----------------------");
    L_INFO("running update loop.");

    auto log_status = [](Client::Status status) {
        std::string status_str = Client::status_to_string(status);
        L_INFO("status: " + status_str);
    };
    auto status = agent.client().status();
    log_status(status);

    while (true) {
        sleep(100);

        if (agent.client().status() == Client::Status::ready) {
            // Randomly tell the game server to become allocated.
            bool shouldSend = std::rand() / ((RAND_MAX + 1u) / 50) == 0;
            if (shouldSend) {
                Array array;
                L_INFO("sending allocated");
                agent.send_allocated(array);
            }
        }

        err = agent.update();
        if (is_error(err)) {
            L_INFO(error_text(err));
            continue;
        }

        auto old_status = status;
        status = agent.client().status();
        if (status != old_status) {
            log_status(status);
        }
    }

    L_INFO("-----------------------");
    L_INFO("agent has been shutdown");
    return 0;
}
