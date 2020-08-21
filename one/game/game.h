#pragma once

#include <functional>
#include <string>

#include <one/game/one_server_wrapper.h>

namespace game {

class OneServerWrapper;

class Game final {
public:
    Game(unsigned int port, int queueLength, int _players, int _max_players,
         const std::string &name, const std::string &map, const std::string &mode,
         const std::string &version);
    Game(const Game &) = delete;
    Game &operator=(const Game &) = delete;
    ~Game();

    int init(size_t max_message_in, size_t max_message_out);
    int shutdown();

    int update();

private:
    OneServerWrapper _server;

    int _players;
    int _max_players;
    std::string _name;
    std::string _map;
    std::string _mode;
    std::string _version;
};

}  // namespace game
