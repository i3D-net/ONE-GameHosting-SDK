#include <one/fake/game/game.h>

#include <one/fake/game/log.h>

#include <chrono>
#include <thread>

using namespace std::chrono;
using namespace one_integration;

void sleep(int ms) {
    std::this_thread::sleep_for(milliseconds(ms));
}

int main(int argc, char **argv) {
    // Init log first for visibility.
    if (argc >= 3) {
        LogCentral::set_log_filename(argv[2]);
    }
    L_INFO("----------------------");
    L_INFO("game startup");

    const unsigned int default_port = 19001;
    unsigned int port = default_port;
    if (argc >= 2) {
        port = strtol(argv[1], nullptr, 10);
        if (port <= 0) {
            L_ERROR("invalid port provided");
            return 1;
        }
    }

    seconds delay(0);

    if (argc >= 4) {
        unsigned int second_delay = strtol(argv[3], nullptr, 10);
        delay = seconds(second_delay);
    }

    if (argc > 4) {
        L_ERROR(
            "invalid number of arguments provided. Maxmium of 3 arguments are "
            "supported.");
        L_ERROR("\t first argument: an integer defining the port number to binds to.");
        L_ERROR(
            "\t second argument: a string defining the log filename to use instead of "
            "logging into std::cout.");
        L_ERROR(
            "\t third argument: an integer defining transition delay in seconds between "
            "starting and online.");
        return -1;
    }

    Game game;
    if (!game.init(port, 16, "test game", "test map", "test mode", "test version",
                   delay)) {
        L_ERROR("failed to init game");
        return 1;
    }
    // Have the process exit when a soft stop is received, after some delay.
    game.set_process_exit_enabled(true);

    L_INFO("game is initialized.");
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
