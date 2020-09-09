#include <one/agent/agent.h>
#include <one/game/log.h>

#include <chrono>
#include <thread>

using namespace std::chrono;
using namespace one;

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

    while (true) {
        sleep(50);

        err = agent.update();
        if (is_error(err)) {
            L_INFO(error_text(err));
            continue;
        }
    }

    L_INFO("-----------------------");
    L_INFO("agent has been shutdown");
    return 0;
}
