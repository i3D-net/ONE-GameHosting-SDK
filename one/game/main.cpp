#include <one/game/game.h>
#include <one/game/log.h>

#include <chrono>
#include <thread>

using namespace std::chrono;
using namespace game;

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

    Game game(port);

    if (!game.init(0, 16, "test game", "test map", "test mode", "test version")) {
        L_ERROR("failed to init game");
        return 1;
    }

    L_INFO("game is initialized.");
    L_INFO("----------------------");
    L_INFO("running update loop.");

    while (true) {
        sleep(50);

        if (!game.update()) {
            L_ERROR("failed to update game");
            return 2;
        }
    }

    L_INFO("----------------------");
    game.shutdown();
    L_INFO("game has been shutdown");

    return 0;
}
