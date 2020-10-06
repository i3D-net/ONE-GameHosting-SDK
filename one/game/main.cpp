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

    if (!game.init(16, "test game", "test map", "test mode", "test version")) {
        L_ERROR("failed to init game");
        return 1;
    }

    L_INFO("game is initialized.");
    L_INFO("----------------------");
    L_INFO("running update loop.");

    auto log_status = [](OneServerWrapper::Status status) {
        std::string status_str = OneServerWrapper::status_to_string(status);
        L_INFO("status: " + status_str);
    };
    auto status = game.one_server_wrapper().status();
    log_status(status);

    while (true) {
        sleep(100);
        game.alter_game_state();
        game.update();

        auto old_status = status;
        status = game.one_server_wrapper().status();
        if (status != old_status) {
            log_status(status);
        }
    }

    L_INFO("----------------------");
    game.shutdown();
    L_INFO("game has been shutdown");

    return 0;
}
